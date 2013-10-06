#include "stdafx.h"
#include <iostream>
#include "data-processing.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

namespace { // anonymous namespace

inline
std::pair<cdmh::data_processing::string_view, cdmh::data_processing::type_mask_t>
read_field(char const *record)
{
    return cdmh::data_processing::detail::read_field(record, record+strlen(record));
}

TEST_CASE("read_field/string fields", "Ensure reading of correct field types")
{
    CHECK(read_field("Hello").second == string_type);
    CHECK(read_field("\"Hello World\"").second == string_type);
    CHECK(read_field("\"Hello \\\"World\\\"!\"").second == string_type);
}

TEST_CASE("read_field/integer fields", "")
{
    CHECK(read_field("8374").second == integer_type);
    CHECK(read_field("837.4").second == double_type);
}

TEST_CASE("read_field/unary signs", "")
{
    CHECK(read_field("+8374").second == integer_type);
    CHECK(read_field("+837.4").second == double_type);
    CHECK(read_field("-8374").second == integer_type);
    CHECK(read_field("-837.4").second == double_type);
}

TEST_CASE("read_field/string fields starting with numerics", "")
{
    CHECK(read_field("83.7.4").second == string_type);
    CHECK(read_field("+83.7.4").second == string_type);
    CHECK(read_field("83a4").second == string_type);
    CHECK(read_field("8.3a4").second == string_type);
    CHECK(read_field("a8.34").second == string_type);
}

TEST_CASE("read_field/numerics with padding", "")
{
    CHECK(read_field("8374 ").second == integer_type);
    CHECK(read_field("+8374 ").second == integer_type);
    CHECK(read_field("-8374 ").second == integer_type);
    CHECK(read_field(" +8374").second == integer_type);
    CHECK(read_field(" +8374 ").second == integer_type);
}

TEST_CASE("read_field/comma separated fields with space padding", "")
{
    auto record = "      Hello, World   ";
    auto it = record;
    auto ite = record+strlen(record);
    auto field1 = cdmh::data_processing::detail::read_field(it, ite);
    auto field2 = cdmh::data_processing::detail::read_field(it, ite);
    CHECK(field1.second == string_type);
    CHECK(field1.first.length() == 5);
    CHECK(field2.second == string_type);
    CHECK(field2.first.length() == 5);
}

TEST_CASE("read_field/spaces around quoted string with leading & trailing spaces", "")
{
    auto record = "    \"  Hello, World \"  ";
    auto it = record;
    auto ite = record+strlen(record);
    auto field = cdmh::data_processing::detail::read_field(it, ite);
    CHECK(std::distance(field.first.begin(), field.first.end()) == 15);
}




TEST_CASE("mapped_csv", "")
{
    cdmh::data_processing::mapped_csv csv("data/training.csv");

#ifdef NDEBUG
    size_t const rows_requested = 0;
    size_t const rows_expected  = 7049;
#else
    size_t const rows_requested = 220;
    size_t const rows_expected  = rows_requested;
#endif

    REQUIRE(csv.read(rows_requested));
    REQUIRE(csv.size() == rows_expected);

    auto ds = csv.create_dataset();
    std::cout << ds.rows() << " records with " << ds.columns() << " columns\n";
    CHECK(ds.rows() == rows_expected);
    REQUIRE(ds.columns() == 31);

    char const *image = ds[0][30];
    image = ds[1][30];   // access row data
    image = ds[2][30];
    image = ds.row(3)[30];

    std::ostringstream stream;
    auto a = ds[3];
    stream << a[0] << " " << a[1] << " ";       // test value serialisation
    stream << ds[210];                   // test row serialisation

    REQUIRE(ds.column(0).count() == ds.rows());      // column 0 has no null values
    REQUIRE(ds.column(28).count() == ds.rows()-1);   // column 28 has a null value
    REQUIRE(ds.column(29).count() == ds.rows()-1);   // column 29 has a null value

    // the column mean ignores null values, so will always be greater
    REQUIRE(ds.column(28).mean() > ds.column(28).sum<double>() / ds.rows());

    std::cout << "\n";
}

}   // anonymous namespace

int main(int argc, char * const argv[])
{
#if defined(_MSC_VER)  &&  defined(_DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    Catch::Session session;
    Catch::ConfigData &config_data = session.configData();
    config_data.showDurations = Catch::ShowDurations::OrNot::Always;

    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0)
        return returnCode;

    return session.run();
}
