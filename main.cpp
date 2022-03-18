
#include <filesystem>
#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <chrono>
#include <ctime>

struct data_struct
{
#ifdef _DEBUG
	const char *output_filename = "enum_bindings.txt";
#else
	const char *output_filename = "..\\scripts\\enum_bindings\\enum_bindings.gml";
#endif

	const char *dragon_output_filename = "enum_bindings.gml";

	const char *extension = ".gml";

	const char *code = R"=====(function enum_binding( _name, _value )
{
	global.enum_binds[$ _name ] = _value;
	global.enum_binds[$ string_to( _name, "." ) + "." + string( _value ) ] = _name;
}

function enum_binding_get_value( _name )
{
	/// @func enum_binding_get_value( name )
	/// @arg	{string}	enum_type_name
	/// @arg	{string}	enum_enty_name

	gml_pragma( "forceinline" );
	if ( DEBUG_MODE_ENABLED )
	{
		assert( variable_struct_exists( global.enum_binds, _name ), string( _name ) + " doesn't exist." );
	}
	return global.enum_binds[$ _name ];
}

function enum_binding_get_name( _type, _value )
{
	/// @func enum_binding_get_name( type, value )
	/// @arg	{string}	enum_type_name
	/// @arg	{ENUM}		enum_value
	/// @ret	{string}	enum_enty_name

	gml_pragma( "forceinline" );
	if ( DEBUG_MODE_ENABLED )
	{
		assert( variable_struct_exists( global.enum_binds, _type + "." + string( _value ) ), _type + "." + string( _value ) + " doesn't exist." );
	}
	return global.enum_binds[$ _type + "." + string( _value ) ];
}

global.enum_binds = {};
)=====";

	std::vector<std::string> files;
} data;

namespace fs = std::filesystem;

inline std::string &ltrim( std::string &s, const char *t = " \t\n\r\f\v" )
{
	s.erase( 0, s.find_first_not_of( t ) );
	return s;
}

inline std::string &rtrim( std::string &s, const char *t = " \t\n\r\f\v" )
{
	s.erase( s.find_last_not_of( t ) + 1 );
	return s;
}

inline std::string &trim( std::string &s, const char *t = " \t\n\r\f\v" )
{
	return ltrim( rtrim( s, t ), t );
}

std::string read_file_into_string( const std::string &path )
{
	auto ss = std::ostringstream{};

	std::ifstream input_file( path );

	if ( !input_file.is_open() )
	{
		std::cerr << "Could not open the file :'" << path << "'" << std::endl;
		exit( EXIT_FAILURE );
	}
	else
	{
		std::cout << "Reading file : '" << path << "'" << std::endl;
	}

	ss << input_file.rdbuf();

	return ss.str();
}

std::string & remove_comments_and_values( std::string &input )
{
	// remove everything after the comment until the end of the line
	auto comment_position = input.find( "//" );
	while ( comment_position != std::string::npos )
	{
		auto comment_end_position = input.find_first_of( "\t\n\r", comment_position );
		if ( comment_end_position != std::string::npos )
			input.erase( comment_position, comment_end_position - comment_position );
		else
			input.erase( comment_position, std::string::npos );
		comment_position = input.find( "//" );
	}

	// remove everything between multiline comments
	auto multiline_comment_position = input.find( "/*" );
	while ( multiline_comment_position != std::string::npos )
	{
		auto multiline_comment_end_position = input.find( "*/", multiline_comment_position );
		if ( multiline_comment_end_position != std::string::npos )
			input.erase( multiline_comment_position, ( multiline_comment_end_position - multiline_comment_position ) + 2 );
		else
			input.erase( multiline_comment_position, std::string::npos );
		multiline_comment_position = input.find( "/*" );
	}

	// remove values [ between = and , or } ]
	auto value_position = input.find( "=" );
	while ( value_position != std::string::npos )
	{
		auto value_end_position = input.find( ",", value_position );
		if ( value_end_position != std::string::npos )
			input.erase( value_position, value_end_position - value_position );
		else
			input.erase( value_position, std::string::npos );
		value_position = input.find( "=" );
	}

	return input;
}

std::vector<std::string> split_string( std::string input )
{
	std::vector<std::string> result;

	std::stringstream ss( remove_comments_and_values( input ) );

	std::string name = "";

	while ( ss.good() )
	{
		std::getline( ss, name );

		trim( name );

		const auto found = name.find_first_of( ",= \t\n\r" );
		if ( found != std::string::npos )
		{
			result.push_back( name.substr( 0, found ) );
		}
		else if ( name != "" )
		{
			result.push_back( name );
		}
	}

	return result;
}

std::string get_file_name( const std::string &input )
{
	auto path = fs::path( input );
	auto parent_path = path.parent_path().string();
	auto name = path.filename().string();
	auto found = parent_path.find_last_of( "/\\" );

	if ( found != std::string::npos )
	{
		parent_path.erase( 0, found + 1 );

		if ( parent_path != name )
		{
			return parent_path + "\\" + name;
		}
	}

	return name;
}

std::string get_time_string()
{
	auto start = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	char buffer[ 30 ];
	ctime_s( buffer, sizeof( buffer ), &start );
	return buffer;
}

// ---------------------------------------------------------------------------------------------------------------------------
// ENTRY
// ---------------------------------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
	auto start = std::chrono::system_clock::now();

	std::cout << std::endl << "Running " << argv[ 0 ] << std::endl << std::endl;

	std::ofstream output_file;

	if ( argc > 1 )
	{
		// specific files were given
		for ( int i = 1; i < argc; ++i )
			if ( fs::path( argv[ i ] ).extension() == data.extension )
				data.files.push_back( argv[ i ] );

		output_file.open( data.dragon_output_filename );
	}
	else
	{
#ifdef _DEBUG
		auto recursive_iter = fs::recursive_directory_iterator( std::filesystem::current_path() );
#else
		auto recursive_iter = fs::recursive_directory_iterator( std::filesystem::current_path().parent_path() );
#endif

		// find all gml files in this directory and subfolders
		for ( const auto &entry : recursive_iter )
		{
			const auto path = entry.path();

			if ( path.extension() == data.extension )
				data.files.push_back( path.string() );
		}

		output_file.open( data.output_filename );
	}

	output_file << std::endl;
	output_file << "// ---------------------------------------------------" << std::endl;
	output_file << "// EnumNameMaker by Azenris, @AzenrisGamer" << std::endl;
	output_file << "// Auto generated enum bindings file : " << std::endl;
	output_file << "// " << get_time_string();
	output_file << "// ---------------------------------------------------" << std::endl;

	output_file << std::endl << data.code << std::endl;

	for ( const auto &entry : data.files )
	{
		std::string string_read = read_file_into_string( entry );

		std::regex regexp( R"(enum\s+([a-zA-Z_]+[a-zA-Z0-9_]*?)\s*\{\s*([\s\S]*?)\s*\})" );
		std::smatch m;

		std::string::const_iterator search_start( string_read.cbegin() );

		std::vector<std::string> bindings;

		// gather the bindings required
		while ( std::regex_search( search_start, string_read.cend(), m, regexp ) )
		{
			auto enum_name = m[ 1 ].str();
			auto results = split_string( m[ 2 ].str() );

			for ( int v = 0; v < results.size(); ++v )
			{
				std::string name = enum_name + "." + results[ v ];

				bindings.push_back( "enum_binding( \"" + name + "\", " + name + " );" );
			}

			search_start = m.suffix().first;
		}

		// output the bindings to file
		if ( bindings.size() > 0 )
		{
			output_file << "// File: " << get_file_name( entry ) << " [ enum binds : " << bindings.size() << " ]" << std::endl;

			for ( auto & binding : bindings )
			{
				output_file << binding << std::endl;
			}

			output_file << std::endl;
		}
	}

	output_file.close();

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << std::endl << "Finished..." << std::endl << "Elapsed time: " << elapsed_seconds.count() << "s" << std::endl << std::endl;

	// ---------------------------------
#if _DEBUG
	system( "pause" );
#endif

	return 0;
}