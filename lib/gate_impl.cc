/* -*- c++ -*- */
/*
 * Copyright 2023 Xin Liu.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <sys/time.h>
#include <gnuradio/io_signature.h>
#include "gate_impl.h"


namespace gr {
	namespace rfid {

		gate::sptr
		gate::make(int sample_rate)
		{
			return gnuradio::get_initial_sptr
				(new gate_impl(sample_rate));
		}
		/*
		 * The private constructor
		 */
		gate_impl::gate_impl(int sample_rate): gr::block("gate",
			gr::io_signature::make(1, 1, sizeof(gr_complex)),
			gr::io_signature::make(1, 1, sizeof(gr_complex))),
			n_samples(0), win_index(0), dc_index(0), num_pulses(0), signal_state(NEG_EDGE), avg_ampl(0), dc_est(0,0)
		{
			// Time from Interrogator transmission to Tag response (250 us)
			n_samples_T1		= T1_D		* (sample_rate / pow(10,6));	// 240 us * (400khz / 1M) = 96 samples
			std::cout << "T1 is the duration from Interrogator transmission to Tag response (250 us). The number of samples in T1 is " << n_samples_T1 << std::endl;

			n_samples_PW		= PW_D		* (sample_rate / pow(10,6));	// 12 us * (400khz / 1M) = 4 samples (not 4.8)
			std::cout << "PW samples : " << n_samples_PW << std::endl;

			// how many samples can represent a tag bit
			n_samples_TAG_BIT	= TAG_BIT_D * (sample_rate / pow(10,6));	// 25 us * (400khz / 1M) = 10 samples
			std::cout << "Samples of 1 Tag bit (40kHz): " << n_samples_TAG_BIT << std::endl;


			win_length = WIN_SIZE_D * (sample_rate/ pow(10,6)); // 250 us * (400khz / 1M) = 100 samples
			std::cout << "Size of window : " << win_length << std::endl;

		
			std::cout << "Duration of window for dc offset estimation : " << DC_SIZE_D << std::endl;
			
			dc_length  = DC_SIZE_D  * (sample_rate / pow(10,6));// 120 us * (400khz / 1M) = 48 samples)
			std::cout << "Size of window for dc offset estimation : " << dc_length << std::endl;

			win_samples.resize(win_length);
			dc_samples.resize(dc_length);

			std::stringstream log_message;

			// First block to be scheduled
			std::cout << "Initializing reader state..." << std::endl;
			
			initialize_reader_state();
		} 

		/*
		 * Our virtual destructor.
		 */
		gate_impl::~gate_impl()
		{
		}

		void
		gate_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
		{
				ninput_items_required[0] = noutput_items;
		}

		int
		gate_impl::general_work (int noutput_items,
											 gr_vector_int &ninput_items,
											 gr_vector_const_void_star &input_items,
											 gr_vector_void_star &output_items)
		{

			const gr_complex *in = (const gr_complex *) input_items[0];
			gr_complex *out = (gr_complex *) output_items[0];

			int n_items = ninput_items[0];
			int number_samples_consumed = n_items;
			float sample_ampl = 0;
			int written = 0;

			
			if( (reader_state-> reader_stats.n_queries_sent   > MAX_NUM_QUERIES ||
					 reader_state-> reader_stats.tag_reads.size() > NUMBER_UNIQUE_TAGS) &&  
					 reader_state-> status != TERMINATED)
			{
				std::cout << " TERMINATED!!!" << std::endl;
				reader_state-> status = TERMINATED;
				gettimeofday (&reader_state-> reader_stats.end, NULL);
				std::cout << "| Execution time : " << reader_state-> reader_stats.end.tv_sec - reader_state-> reader_stats.start.tv_sec << " seconds" << std::endl;
				GR_LOG_INFO(d_logger, "Termination");
			 }

			// Gate block is controlled by the Gen2 Logic block
			if(reader_state->gate_status == GATE_SEEK_EPC)
			{
				// std::cout << " GATE_SEEK_EPC!!!" << std::endl;
				reader_state->gate_status = GATE_CLOSED;
				reader_state->n_samples_to_ungate = (EPC_BITS + TAG_PREAMBLE_BITS) * n_samples_TAG_BIT + 2*n_samples_TAG_BIT;
				n_samples = 0;
			}
			else if (reader_state->gate_status == GATE_SEEK_RN16)
			{
				// std::cout << " GATE_SEEK_RN16!!!" << std::endl;
				reader_state->gate_status = GATE_CLOSED;
				reader_state->n_samples_to_ungate = (RN16_BITS + TAG_PREAMBLE_BITS) * n_samples_TAG_BIT + 2*n_samples_TAG_BIT;
				n_samples = 0;
			}
			
			if (reader_state->status == RUNNING)
			{
				// std::cout << " RUNNING!!!" << std::endl;
				for(int i = 0; i < n_items; i++)
				{
					// Tracking average amplitude
					// std::cout << n_items << std::endl;
					sample_ampl = std::abs(in[i]);
					avg_ampl = avg_ampl + (sample_ampl - win_samples[win_index])/win_length;  
					win_samples[win_index] = sample_ampl; 
					win_index = (win_index + 1) % win_length;
	
					//Threshold for detecting negative/positive edges
					sample_thresh = avg_ampl * THRESH_FRACTION;  

					if( !(reader_state->gate_status == GATE_OPEN) )
					{
						// std::cout << " GATE_OPEN!!!" << std::endl;
						
						//Tracking DC offset (only during T1)
						dc_est =  dc_est + (in[i] - dc_samples[dc_index])/std::complex<float>(dc_length,0);  
						dc_samples[dc_index] = in[i]; 
						dc_index = (dc_index + 1) % dc_length;
					
						n_samples++;

						// Potitive edge -> Negative edge
						if( sample_ampl < sample_thresh && signal_state == POS_EDGE)
						{
							n_samples = 0;
							signal_state = NEG_EDGE;
						}
						// Negative edge -> Positive edge 
						else if (sample_ampl > sample_thresh && signal_state == NEG_EDGE)
						{
							signal_state = POS_EDGE;
							if (n_samples > n_samples_PW/2)
								num_pulses++; 
							else
								num_pulses = 0; 
							n_samples = 0;
						}
						// std::cout <<  n_samples << std::endl;
						if(n_samples > n_samples_T1 && signal_state == POS_EDGE && num_pulses > NUM_PULSES_COMMAND)
						{
							// std::cout << " READER COMMAND DETECTED!!!" << std::endl;
							GR_LOG_INFO(d_debug_logger, "READER COMMAND DETECTED");

							reader_state->gate_status = GATE_OPEN;

							reader_state->magn_squared_samples.resize(0);


							reader_state->magn_squared_samples.push_back(std::norm(in[i] - dc_est));
							out[written] = in[i] - dc_est;  
							written++;

							num_pulses = 0; 
							n_samples =  1; // Count number of samples passed to the next block

						}
					}
					else
					{
						
						// std::cout << " ELSE!!!" << std::endl;
						n_samples++;

						reader_state->magn_squared_samples.push_back(std::norm(in[i] - dc_est));
						out[written] = in[i] - dc_est; // Remove offset from complex samples           
						written++;
						if (n_samples >= reader_state->n_samples_to_ungate)
						{
							reader_state->gate_status = GATE_CLOSED;    
							number_samples_consumed = i+1;
							break;
						}
					}
				}
			}
			consume_each (number_samples_consumed);
			return written;
		}
	} /* namespace rfid */
} /* namespace gr */
