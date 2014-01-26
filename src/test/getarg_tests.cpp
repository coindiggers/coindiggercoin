#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CDC");
    BOOST_CHECK(GetBoolArg("-CDC"));
    BOOST_CHECK(GetBoolArg("-CDC", false));
    BOOST_CHECK(GetBoolArg("-CDC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CDCo"));
    BOOST_CHECK(!GetBoolArg("-CDCo", false));
    BOOST_CHECK(GetBoolArg("-CDCo", true));

    ResetArgs("-CDC=0");
    BOOST_CHECK(!GetBoolArg("-CDC"));
    BOOST_CHECK(!GetBoolArg("-CDC", false));
    BOOST_CHECK(!GetBoolArg("-CDC", true));

    ResetArgs("-CDC=1");
    BOOST_CHECK(GetBoolArg("-CDC"));
    BOOST_CHECK(GetBoolArg("-CDC", false));
    BOOST_CHECK(GetBoolArg("-CDC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCDC");
    BOOST_CHECK(!GetBoolArg("-CDC"));
    BOOST_CHECK(!GetBoolArg("-CDC", false));
    BOOST_CHECK(!GetBoolArg("-CDC", true));

    ResetArgs("-noCDC=1");
    BOOST_CHECK(!GetBoolArg("-CDC"));
    BOOST_CHECK(!GetBoolArg("-CDC", false));
    BOOST_CHECK(!GetBoolArg("-CDC", true));

    ResetArgs("-CDC -noCDC");  // -CDC should win
    BOOST_CHECK(GetBoolArg("-CDC"));
    BOOST_CHECK(GetBoolArg("-CDC", false));
    BOOST_CHECK(GetBoolArg("-CDC", true));

    ResetArgs("-CDC=1 -noCDC=1");  // -CDC should win
    BOOST_CHECK(GetBoolArg("-CDC"));
    BOOST_CHECK(GetBoolArg("-CDC", false));
    BOOST_CHECK(GetBoolArg("-CDC", true));

    ResetArgs("-CDC=0 -noCDC=0");  // -CDC should win
    BOOST_CHECK(!GetBoolArg("-CDC"));
    BOOST_CHECK(!GetBoolArg("-CDC", false));
    BOOST_CHECK(!GetBoolArg("-CDC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CDC=1");
    BOOST_CHECK(GetBoolArg("-CDC"));
    BOOST_CHECK(GetBoolArg("-CDC", false));
    BOOST_CHECK(GetBoolArg("-CDC", true));

    ResetArgs("--noCDC=1");
    BOOST_CHECK(!GetBoolArg("-CDC"));
    BOOST_CHECK(!GetBoolArg("-CDC", false));
    BOOST_CHECK(!GetBoolArg("-CDC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CDC", "eleven"), "eleven");

    ResetArgs("-CDC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CDC", "eleven"), "");

    ResetArgs("-CDC=");
    BOOST_CHECK_EQUAL(GetArg("-CDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CDC", "eleven"), "");

    ResetArgs("-CDC=11");
    BOOST_CHECK_EQUAL(GetArg("-CDC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CDC", "eleven"), "11");

    ResetArgs("-CDC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CDC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CDC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CDC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CDC", 0), 0);

    ResetArgs("-CDC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CDC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CDC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CDC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CDC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CDC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CDC");
    BOOST_CHECK_EQUAL(GetBoolArg("-CDC"), true);

    ResetArgs("--CDC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CDC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCDC");
    BOOST_CHECK(!GetBoolArg("-CDC"));
    BOOST_CHECK(!GetBoolArg("-CDC", true));
    BOOST_CHECK(!GetBoolArg("-CDC", false));

    ResetArgs("-noCDC=1");
    BOOST_CHECK(!GetBoolArg("-CDC"));
    BOOST_CHECK(!GetBoolArg("-CDC", true));
    BOOST_CHECK(!GetBoolArg("-CDC", false));

    ResetArgs("-noCDC=0");
    BOOST_CHECK(GetBoolArg("-CDC"));
    BOOST_CHECK(GetBoolArg("-CDC", true));
    BOOST_CHECK(GetBoolArg("-CDC", false));

    ResetArgs("-CDC --noCDC");
    BOOST_CHECK(GetBoolArg("-CDC"));

    ResetArgs("-noCDC -CDC"); // CDC always wins:
    BOOST_CHECK(GetBoolArg("-CDC"));
}

BOOST_AUTO_TEST_SUITE_END()
