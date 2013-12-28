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
    ResetArgs("-grdtcoin");
    BOOST_CHECK(GetBoolArg("-grdtcoin"));
    BOOST_CHECK(GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(GetBoolArg("-grdtcoin", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-grdtcoino"));
    BOOST_CHECK(!GetBoolArg("-grdtcoino", false));
    BOOST_CHECK(GetBoolArg("-grdtcoino", true));

    ResetArgs("-grdtcoin=0");
    BOOST_CHECK(!GetBoolArg("-grdtcoin"));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", true));

    ResetArgs("-grdtcoin=1");
    BOOST_CHECK(GetBoolArg("-grdtcoin"));
    BOOST_CHECK(GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(GetBoolArg("-grdtcoin", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nogrdtcoin");
    BOOST_CHECK(!GetBoolArg("-grdtcoin"));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", true));

    ResetArgs("-nogrdtcoin=1");
    BOOST_CHECK(!GetBoolArg("-grdtcoin"));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", true));

    ResetArgs("-grdtcoin -nogrdtcoin"); // -grdtcoin should win
    BOOST_CHECK(GetBoolArg("-grdtcoin"));
    BOOST_CHECK(GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(GetBoolArg("-grdtcoin", true));

    ResetArgs("-grdtcoin=1 -nogrdtcoin=1"); // -grdtcoin should win
    BOOST_CHECK(GetBoolArg("-grdtcoin"));
    BOOST_CHECK(GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(GetBoolArg("-grdtcoin", true));

    ResetArgs("-grdtcoin=0 -nogrdtcoin=0"); // -grdtcoin should win
    BOOST_CHECK(!GetBoolArg("-grdtcoin"));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--grdtcoin=1");
    BOOST_CHECK(GetBoolArg("-grdtcoin"));
    BOOST_CHECK(GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(GetBoolArg("-grdtcoin", true));

    ResetArgs("--nogrdtcoin=1");
    BOOST_CHECK(!GetBoolArg("-grdtcoin"));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", false));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", "eleven"), "eleven");

    ResetArgs("-grdtcoin -bar");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", "eleven"), "");

    ResetArgs("-grdtcoin=");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", "eleven"), "");

    ResetArgs("-grdtcoin=11");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", "eleven"), "11");

    ResetArgs("-grdtcoin=eleven");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", 0), 0);

    ResetArgs("-grdtcoin -bar");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-grdtcoin=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-grdtcoin=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--grdtcoin");
    BOOST_CHECK_EQUAL(GetBoolArg("-grdtcoin"), true);

    ResetArgs("--grdtcoin=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-grdtcoin", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nogrdtcoin");
    BOOST_CHECK(!GetBoolArg("-grdtcoin"));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", true));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", false));

    ResetArgs("-nogrdtcoin=1");
    BOOST_CHECK(!GetBoolArg("-grdtcoin"));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", true));
    BOOST_CHECK(!GetBoolArg("-grdtcoin", false));

    ResetArgs("-nogrdtcoin=0");
    BOOST_CHECK(GetBoolArg("-grdtcoin"));
    BOOST_CHECK(GetBoolArg("-grdtcoin", true));
    BOOST_CHECK(GetBoolArg("-grdtcoin", false));

    ResetArgs("-grdtcoin --nogrdtcoin");
    BOOST_CHECK(GetBoolArg("-grdtcoin"));

    ResetArgs("-nogrdtcoin -grdtcoin"); // grdtcoin always wins:
    BOOST_CHECK(GetBoolArg("-grdtcoin"));
}

BOOST_AUTO_TEST_SUITE_END()
