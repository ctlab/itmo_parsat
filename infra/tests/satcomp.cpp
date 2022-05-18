#include "infra/fixture/LaunchFixture.h"

#include <sstream>
#include <filesystem>

#include "core/tests/common/util.h"
#include "util/visit.h"

INSTANTIATE_TEST_SUITE_P(
    SatcompCommonBenchmarks, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(std::vector<std::string>{
        "satcomp/ms-rr-base",
        "satcomp/ms-rr-more-sharing",
        "satcomp/ms-rr-one-reducer",
        "satcomp/ms-rr-rec_5m",
        "satcomp/ms-rr-worse-backdoors",
        "satcomp/ps-sc-base",
        "satcomp/ps-sc-prod_2",
        "satcomp/ps-sc-prod_many_small",
    })));
