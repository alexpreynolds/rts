#ifndef RTS_HPP_
#define RTS_HPP_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif /* getline() support */

#define MAX_NAME_LENGTH 4096
#define MAX_BIT_WIDTH 16
#define MAX_PERCENTAGE_WIDTH 3

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

	#include <string>
	#include <vector>
	#include <stdexcept>
	#include <exception>
	#include <cmath>
	#include <cstring>
	#include <climits>
	#include <iostream>
	#include <sstream>
	#include <getopt.h>
	#include <time.h>
	#include <random>
	#include <unordered_set>
	#include <iomanip>

	namespace rts
	{
		class RTS
		{
		private:
			std::vector<unsigned char> _bitset;
			long _nbytes;
			long _real_bits;
			long _max_bits;
			int _rows;
			int _cols;
			int _samples;
			int _rng_seed;
			bool _rng_seed_specified;
			bool _preserve_metadata;
			int _order;
			std::mt19937 _mt19937_uint_gen;
			std::vector<std::string> _row_names;
			std::vector<std::string> _col_names;
			bool _track_conversion;
			int _bits_perc;

			inline long
			_byte_offset_from_rc(const int& r, const int& c)
			{
				long o = 0;
				long m = (r * c) + c;
				while (m > 0) {
					m -= CHAR_BIT;
					if (m >= 0) {
						o++;
					}
				}
				return o;
			}

			inline long
			_byte_offset_from_index(const int& idx)
			{
				long o = 0;
				long m = idx;
				while (m > 0) {
					m -= CHAR_BIT;
					if (m >= 0) {
						o++;
					}
				}
				return o;
			}

	  	public:
			static const std::string name;
			static const std::string version;
			static const std::string authors;

			typedef enum
			{
				Undefined = 0,
				UpperTriangular,
				LowerTriangular
			} SquareMatrixType;

			SquareMatrixType smt;

			std::vector<unsigned char>& bitset() { return _bitset; }
			long nbytes() const { return _nbytes; }
			long max_bits() const { return _max_bits; }
			long real_bits() const { return _real_bits; }
			int rows() const { return _rows; }
			void rows(const int& r) { _rows = r; }
			int cols() const { return _cols; }
			void cols(const int& c) { _cols = c; }
			int samples() const { return _samples; }
			void samples(const int& k) { _samples = k; }
			int rng_seed() const { return _rng_seed; }
			void rng_seed(const int& s) { _rng_seed = s; }
			bool rng_seed_specified() const { return _rng_seed_specified; }
			void rng_seed_specified(const bool& f) { _rng_seed_specified = f; }
			int order() const { return _order; }
			void order(const int& o) { _order = o; }
			std::mt19937& mt19937_uint_gen() { return _mt19937_uint_gen; }
			void mt19937_uint_gen(const std::mt19937& g) { _mt19937_uint_gen = g; }
			SquareMatrixType& square_matrix_type() { return smt; }
			void square_matrix_type(const SquareMatrixType& t) { smt = t; }
			bool preserve_metadata() const { return _preserve_metadata; }
			void preserve_metadata(const bool& f) { _preserve_metadata = f; }
			std::vector<std::string>& row_names() { return _row_names; }
			void row_names(const std::vector<std::string>& v) { _row_names = v; }
			std::vector<std::string>& col_names() { return _col_names; }
			void col_names(const std::vector<std::string>& v) { _col_names = v; }
			bool track_conversion() const { return _track_conversion; }
			void track_conversion(const bool& f) { _track_conversion = f; }
			int bits_perc() const { return _bits_perc; }
			void bits_perc(const int& b) { _bits_perc = b; }

			void initialize_command_line_options(int argc, char** argv);
			std::string rts_opt_string(void);
			struct option* rts_long_options(void);
			void print_usage(FILE* os);
			void print_version(FILE* os);
			std::string rts_name(void);
			std::string rts_version(void);
			std::string rts_authors(void);
			std::string rts_usage(void);
			std::string rts_description(void);
			std::string rts_io_options(void);
			std::string rts_general_options(void);

			void initialize_bitset(void);
			void read_metadataless_matrix_into_bitset(void);
			void read_metadata_matrix_into_bitset(void);
			void print_metadataless_bitset(void);
			void print_metadata_bitset(void);
			void sample_metadataless_bitset(void);
			void sample_metadata_bitset(void);

			//
			// Bob Floyd sampling algorithm (via Programming Pearls)
			//
			std::unordered_set<int> 
			sample_order_indices_without_replacement(const int& sample_size, const int& range_upper_bound)
			{
				std::unordered_set<int> sample;
				for(int d = range_upper_bound - sample_size; d < range_upper_bound; d++) {
					int t = std::uniform_int_distribution<>(0, d)(_mt19937_uint_gen);
					if (sample.find(t) == sample.end())
						sample.insert(t);
					else
						sample.insert(d);
				}
				return sample;
			}

			void 
			reserve_bitset(const int& r, const int& c)
			{
				//
				// we need r*c bits to store binary state
				// which requires reserving at least:
				//
				// ceil((r*c)/8) max bytes, or: 8*ceil((r*c)/8) max bits
				//
				// watch out for integer overflow on multiplication
				//
				try {
					if ((r < 1) || (c < 1)) {
						throw std::domain_error("Error: Specify positive non-zero row and column values");
					}
					_rows = r;
					_cols = c;
					_nbytes = long(ceil((long(_rows) * _cols) / 8.0));
					//std::cout << _nbytes << std::endl;
					_bitset.reserve(_nbytes);
					_real_bits = _rows * _cols;
					_max_bits = _nbytes << 3;
					//std::cout << _max_bits << std::endl;
				}
				catch (const std::exception& e) {
					std::cout << e.what() << std::endl;
					std::terminate();
				}
			}

			inline void 
			set_all_bits_to(const bool b) 
			{
				for (long bidx = 0; bidx < _nbytes; ++bidx) { 
					_bitset[bidx] = (b) ? 0xff : 0x00; 
				} 
			}

			void
			set_bit(const int& r, const int& c, const bool& f)
			{
				int x = f ? 1 : 0;
				long o = _byte_offset_from_rc(r, c);
				int s = o % CHAR_BIT;
				_bitset[o] ^= (-x ^ _bitset[o]) & (1UL << s);
			}

			void
			set_byte(const long& o, const unsigned char& c)
			{
				_bitset[o] = c;
			}

			void
			set_bit(const long& idx, const bool& f)
			{
				int x = f ? 1 : 0;
				int s = idx % CHAR_BIT;
				long o = _byte_offset_from_index(idx);
				_bitset[o] ^= (-x ^ _bitset[o]) & (1UL << s);
			}

			bool
			get_bit(const int& r, const int& c)
			{
				long idx = (r * _cols) + c;
				int s = idx % CHAR_BIT;
				long o = _byte_offset_from_index(idx);
				return (_bitset[o] >> s) & 1U;
			}

			const char *
			byte_to_binary(const int& byte)
			{
				static char binary[CHAR_BIT + 1];
				binary[0] = '\0';
				for (int z = 128; z > 0; z >>= 1) {
					std::strcat(binary, ((byte & z) == z) ? "1" : "0");
				}
				return binary;
			}

		RTS();
		~RTS();
		};

		RTS::RTS() {
			rows(-1);
			cols(-1);
			samples(-1);
			rng_seed(-1);
			rng_seed_specified(false);
			order(-1);
			preserve_metadata(false);
			track_conversion(false);
		}

		RTS::~RTS() {
		}
	}

#endif // RTS_HPP_