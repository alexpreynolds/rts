#include "rts.hpp"

const std::string rts::RTS::name = "rts";
const std::string rts::RTS::version = "1.0";
const std::string rts::RTS::authors = "Alex Reynolds";

int
main(int argc, char** argv)
{
	rts::RTS r;

	r.initialize_command_line_options(argc, argv);
	r.initialize_bitset();
	if (r.preserve_metadata()) {
		r.read_metadata_matrix_into_bitset();
		//r.print_metadata_bitset();
		r.sample_metadata_bitset();
	}
	else {
		r.read_metadataless_matrix_into_bitset();
		//r.print_metadataless_bitset();
		r.sample_metadataless_bitset();
	}
	
	return EXIT_SUCCESS;
}

void
rts::RTS::sample_metadata_bitset(void)
{
	for (int sample = 0; sample < this->samples(); ++sample) {
		bool square_matrix_is_of_desired_type = true;
		auto rows_sample = this->sample_order_indices_without_replacement(this->order(), this->rows());
		auto cols_sample = this->sample_order_indices_without_replacement(this->order(), this->cols());
		int synthetic_row = 0;
		for (auto row_iterator = rows_sample.begin(); row_iterator != rows_sample.end(); ++row_iterator, ++synthetic_row) {
			int synthetic_col = 0;
			for (auto col_iterator = cols_sample.begin(); col_iterator != cols_sample.end(); ++col_iterator, ++synthetic_col) {
				bool bit = get_bit(*row_iterator, *col_iterator);
				if (
					bit &&
					(
						((this->square_matrix_type() == RTS::LowerTriangular) && (synthetic_col > synthetic_row)) 
						||	
						((this->square_matrix_type() == RTS::UpperTriangular) && (synthetic_col < synthetic_row))
					)) {
					square_matrix_is_of_desired_type = false;
					break;
				}
			}
		}
		// if matrix is upper or lower triangular, print it
		if (square_matrix_is_of_desired_type) {
			int synthetic_row = 0;
			for (auto row_iterator = rows_sample.begin(); row_iterator != rows_sample.end(); ++row_iterator, ++synthetic_row) {
				int synthetic_col = 0;
				if (synthetic_row == 0) {
					for (auto col_iterator = cols_sample.begin(); col_iterator != cols_sample.end(); ++col_iterator, ++synthetic_col) {
						std::cout << '\t' << this->col_names()[*col_iterator];
					}
					std::cout << std::endl;
				}
				std::cout << this->row_names()[*row_iterator];
				for (auto col_iterator = cols_sample.begin(); col_iterator != cols_sample.end(); ++col_iterator, ++synthetic_col) {
					bool bit = get_bit(*row_iterator, *col_iterator);
					std::cout << '\t' << bit;
				}
				std::cout << std::endl;
			}
		}
	}
}

void
rts::RTS::sample_metadataless_bitset(void)
{
	for (int sample = 0; sample < this->samples(); ++sample) {
		bool square_matrix_is_of_desired_type = true;
		auto rows_sample = this->sample_order_indices_without_replacement(this->order(), this->rows());
		auto cols_sample = this->sample_order_indices_without_replacement(this->order(), this->cols());
		int synthetic_row = 0;
		for (auto row_iterator = rows_sample.begin(); row_iterator != rows_sample.end(); ++row_iterator, ++synthetic_row) {
			int synthetic_col = 0;
			for (auto col_iterator = cols_sample.begin(); col_iterator != cols_sample.end(); ++col_iterator, ++synthetic_col) {
				bool bit = get_bit(*row_iterator, *col_iterator);
				std::cout << "[" << *row_iterator << ", " << *col_iterator << "] ";
				std::cout << "[" << synthetic_row << ", " << synthetic_col << "] ";
				std::cout << bit;
				if (
					bit &&
					(
						((this->square_matrix_type() == RTS::LowerTriangular) && (synthetic_col > synthetic_row)) 
						||	
						((this->square_matrix_type() == RTS::UpperTriangular) && (synthetic_col < synthetic_row))
					)) {
					square_matrix_is_of_desired_type = false;
				}
				std::cout << "     \t";
			}
			std::cout << std::endl;
		}
		std::cout << "Matrix " << (square_matrix_is_of_desired_type ? "is " : "is not ") << (this->square_matrix_type() == RTS::UpperTriangular ? "upper " : "lower ") << "triangular" << std::endl;
		std::cout << "----" << std::endl;
	}
}

void
rts::RTS::print_metadata_bitset(void)
{
	for (int row = -1; row < this->rows(); ++row) {
		if (row == -1) {
			std::cout << '\t';
			for (int col = 0; col < this->cols(); ++col) {
				std::cout << this->col_names()[col] << '\t';
			}
		}
		else {
			std::cout << this->row_names()[row] << '\t';
			for (int col = 0; col < this->cols(); ++col) {
				std::cout << this->get_bit(row, col) << '\t';
			}
		}
		std::cout << std::endl;
	}
}

void
rts::RTS::print_metadataless_bitset(void)
{
	for (int row = 0; row < this->rows(); ++row) {
		for (int col = 0; col < this->cols(); ++col) {
			std::cout << "[" << row << "|" << col << "] " << this->get_bit(row, col) << '\t' << '\t';
		}
		std::cout << std::endl;
	}
}

void
rts::RTS::read_metadata_matrix_into_bitset(void)
{
	long bit_idx = 0;
	long byte = 0;
	long bit = 0;
	std::string token;
	int row_idx = 0;
	int col_idx = 0;
	int increment = 0;
	char* name = NULL;

	name = (char*) malloc(MAX_NAME_LENGTH + 1);
	std::memset(name, 0, MAX_NAME_LENGTH + 1);

	for (std::string line; std::getline(std::cin, line); ) {
		char* p = NULL;
		char* lm = NULL;
		const char* lc = line.c_str();
		lm = const_cast<char*>(lc);
		
		if ((row_idx == 0) && (col_idx >= 0)) {
			// read column name into col_names vector
			p = std::strchr(lm, '\t');
			while (p) {
				std::memcpy(name, lm, p - lm);
				//std::cerr << "found at: " << p - lm << std::endl;
				name[p - lm] = '\0';
				lm = p + 1;
				//std::cerr << "col: " << name << std::endl;
				if (col_idx > 0) this->col_names().push_back(name);
				++col_idx;
				p = (col_idx == this->cols()) ? std::strchr(lm, '\0') : std::strchr(lm, '\t');
			}
			col_idx = 0;
			++row_idx;
			continue;
		}

		if ((row_idx > 0) && (col_idx == 0)) {
			// read row name into row_names vector
			p = std::strchr(lm, '\t');
			std::memcpy(name, lm, p - lm);
			name[p - lm] = '\0';
			//std::cerr << "row: " << name << std::endl;
			this->row_names().push_back(name);
			++col_idx;
			lm = p + 1;
		}

		if ((row_idx > 0) && (col_idx > 0)) {
			// parse 1s and 0s
			while (col_idx <= this->cols()) {
				bool binary_flag = (*lm == '0') ? false : true;
				bit_idx = (byte * CHAR_BIT) + bit;
				this->set_bit(bit_idx, binary_flag);
				if (this->track_conversion()) {
					//std::cerr << row_idx << '|' << col_idx << '|' << *lm << '|' << byte << '|' << bit_idx << '|' << this->get_bit(row_idx-1, col_idx-1) << std::endl;
					if ((bit_idx % this->bits_eighth_perc()) == 0) {
						std::cerr << "..." << round(100.0*increment/this->real_bits()) << " percent done (" << bit_idx << " bits of " << (this->real_bits() - 1) << ")" << std::endl;
					}
					++increment;
				}
				++bit;
				if (bit == CHAR_BIT) {
					bit = 0;
					++byte;
				}
				lm += 2;
				++col_idx;
			}
			col_idx = 0;
			++row_idx;
			continue;
		}
	}
	if (this->track_conversion()) {
		std::cerr << "..." << round(100.0*increment/this->real_bits()) << " percent done (" << bit_idx << " bits of " << (this->real_bits() - 1) << ")" << std::endl;
	}

	free(name);
}

void
rts::RTS::read_metadataless_matrix_into_bitset(void)
{
	long byte = 0;
	long bit = 0;
	std::string token;
	for (std::string line; std::getline(std::cin, line); ) {
		std::istringstream iss(line);
		while (std::getline(iss, token, '\t')) {
			bool f = (std::stoi(token) == 0) ? false : true;
			long idx = (byte * CHAR_BIT) + bit;
			//std::cout << "setting " << token << " to " << f << " at offset " << idx << std::endl;
			this->set_bit(idx, f);
			++bit;
			if (bit == CHAR_BIT) {
				bit = 0;
				++byte;
			}
		}
	}
}

void
rts::RTS::initialize_bitset(void)
{
	this->reserve_bitset(this->rows(), this->cols());
	this->set_all_bits_to(false);

	// tracking info
	if (this->track_conversion()) {
		this->bits_eighth_perc(this->max_bits() / CHAR_BIT);
	}
}

std::string
rts::RTS::rts_opt_string(void)
{
	static std::string _s("r:c:k:s:o:ulpthv?");
	return _s;
}

struct option*
rts::RTS::rts_long_options(void)
{
	static struct option _r = { "rows",                       required_argument,   NULL,    'r' };
	static struct option _c = { "cols",                       required_argument,   NULL,    'c' };
	static struct option _k = { "samples",                    required_argument,   NULL,    'k' };
	static struct option _s = { "rng-seed",                   required_argument,   NULL,    's' };
	static struct option _o = { "order",                      required_argument,   NULL,    'o' };
	static struct option _u = { "upper",                      no_argument,         NULL,    'u' };
	static struct option _l = { "lower",                      no_argument,         NULL,    'l' };
	static struct option _p = { "preserve-metadata",          no_argument,         NULL,    'p' };
	static struct option _t = { "track-conversion",           no_argument,         NULL,    't' };
	static struct option _h = { "help",                       no_argument,         NULL,    'h' };
	static struct option _v = { "version",                    no_argument,         NULL,    'v' };
	static struct option _0 = { NULL,                         no_argument,         NULL,     0  };
	static std::vector<struct option> _opts;
	_opts.push_back(_r);
	_opts.push_back(_c);
	_opts.push_back(_k);
	_opts.push_back(_s);
	_opts.push_back(_o);
	_opts.push_back(_u);
	_opts.push_back(_l);
	_opts.push_back(_p);
	_opts.push_back(_t);
	_opts.push_back(_h);
	_opts.push_back(_v);
	_opts.push_back(_0);
	return &_opts[0];
}

void
rts::RTS::initialize_command_line_options(int argc, char** argv)
{
	int client_long_index;
	int client_opt = getopt_long(argc,
								 argv,
								 this->rts_opt_string().c_str(),
								 this->rts_long_options(),
								 &client_long_index);
	int _r = -1;
	int _c = -1;
	int _k = -1;
	int _s = -1;
	int _o = -1;
	bool is_upper_set = false;
	bool is_lower_set = false;

	opterr = 0; /* disable error reporting by GNU getopt */

	while (client_opt != -1) {
		switch (client_opt) {
		case 'r':
			std::sscanf(optarg, "%d", &_r);
			this->rows(_r);
			break;
		case 'c':
			std::sscanf(optarg, "%d", &_c);
			this->cols(_c);
			break;
		case 'k':
			std::sscanf(optarg, "%d", &_k);
			this->samples(_k);
			break;
		case 's':
			std::sscanf(optarg, "%d", &_s);
			this->rng_seed(_s);
			this->rng_seed_specified(true);
			break;
		case 'o':
			std::sscanf(optarg, "%d", &_o);
			this->order(_o);
			break;
		case 'u':
			this->square_matrix_type(RTS::UpperTriangular);
			is_upper_set = true;
			break;
		case 'l':
			this->square_matrix_type(RTS::LowerTriangular);
			is_lower_set = true;
			break;
		case 'p':
			this->preserve_metadata(true);
			break;
		case 't':
			this->track_conversion(true);
			break;
		case 'h':
			this->print_usage(stdout);
			std::exit(EXIT_SUCCESS);
		case 'v':
			this->print_version(stdout);
			std::exit(EXIT_SUCCESS);
		case '?':
			this->print_usage(stdout);
			std::exit(EXIT_SUCCESS);
		default:
			break;
		}
		client_opt = getopt_long(argc,
								 argv,
								 this->rts_opt_string().c_str(),
								 this->rts_long_options(),
								 &client_long_index);
	}

	bool error_flagged = false;
  
	if (this->rows() == -1) {
		std::fprintf(stderr, "Error: Specify --rows (rows) value\n");
		error_flagged = true;
	}

	if (this->cols() == -1) {
		std::fprintf(stderr, "Error: Specify --cols (columns) value\n");
		error_flagged = true;
	}

	if (this->samples() == -1) {
		std::fprintf(stderr, "Error: Specify --samples (number of samples) value\n");
		error_flagged = true;
	}

	if (this->order() == -1) {
		std::fprintf(stderr, "Error: Specify --order (order of submatrix) value\n");
		error_flagged = true;
	}

	if (is_upper_set == is_lower_set) {
		std::fprintf(stderr, "Error: Specify either --upper or --lower (square matrix type)\n");
		error_flagged = true;
	}

	if ((this->order() > this->rows()) || (this->order() > this->cols())) {
		std::fprintf(stderr, "Error: Order must be less than or equal to number of specified rows and columns\n");
		error_flagged = true;
	}

	if (error_flagged) {
		this->print_usage(stderr);
		std::exit(ENODATA);
	}

	// initialize RNG
	std::mt19937 gen(this->rng_seed_specified() ? this->rng_seed() : time(NULL));
 	this->mt19937_uint_gen(gen);

 	// initialize metadata
 	if (this->preserve_metadata()) {
 		this->row_names().reserve(this->rows());
 		this->col_names().reserve(this->cols());
 	}
}

std::string
rts::RTS::rts_name(void)
{
	static std::string _s(rts::RTS::name);
	return _s;
}

std::string
rts::RTS::rts_version(void)
{
	static std::string _s(rts::RTS::version);
	return _s;
}

std::string
rts::RTS::rts_authors(void)
{
	static std::string _s(rts::RTS::authors);
	return _s;
}

std::string
rts::RTS::rts_usage(void)
{
	static std::string _s("\n"						\
						  "  Usage:\n"					\
						  "\n"						\
						  "  $ rts [arguments] < input\n");
	return _s;
}

std::string
rts::RTS::rts_description(void)
{
	static std::string _s("  \n");
	return _s;
}

std::string
rts::RTS::rts_io_options(void)
{
	static std::string _s("  General Options:\n\n"              \
						  "  --rows=n                       Rows (integer, required)\n" \
						  "  --cols=n                       Columns (integer, required)\n\n" \
						  "  If --preserve-metadata is used, only specify number of rows and\n" \
						  "  columns used for data values.\n\n"
						  "  --samples=n                    Number of random draws (integer, required)\n" \
						  "  --order=n                      Order of triangular matrix (integer, required)\n" \
						  "  --upper | --lower              Type of triangular matrix (required)\n" \
						  "  --preserve-metadata            Parse and print row and column names with samples (optional)\n" \
						  "  --track-conversion             Track progress of conversion to bits (optional)\n" \
						  "  --rng-seed=n                   RNG seed (integer, optional)\n" );
	return _s;
}

std::string
rts::RTS::rts_general_options(void)
{
	static std::string _s("  Process Flags:\n\n"				\
						  "  --help                         Show this usage message\n" \
						  "  --version                      Show binary version\n");
	return _s;
}

void
rts::RTS::print_usage(FILE* os)
{
	std::fprintf(os,
				 "%s\n"						     \
				 "  version: %s\n"				 \
				 "  author:  %s\n"				 \
				 "%s\n"						     \
				 "%s\n"						     \
				 "%s\n"						     \
				 "%s\n",
				 this->rts_name().c_str(),
				 this->rts_version().c_str(),
				 this->rts_authors().c_str(),
				 this->rts_usage().c_str(),
				 this->rts_description().c_str(),
				 this->rts_io_options().c_str(),
				 this->rts_general_options().c_str());
}

void
rts::RTS::print_version(FILE* os)
{
	std::fprintf(os,
				 "%s\n"						     \
				 "  version: %s\n"				 \
				 "  author:  %s\n",
				 this->rts_name().c_str(),
				 this->rts_version().c_str(),
				 this->rts_authors().c_str());
}