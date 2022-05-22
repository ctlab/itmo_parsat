#include "infra/fixture/LaunchFixture.h"

#include <sstream>
#include <filesystem>

#include "core/tests/common/util.h"
#include "util/visit.h"

INSTANTIATE_TEST_SUITE_P(
    SatcompCommonBenchmarks, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(std::vector<std::string>{
        "scaling/a-1",
        "scaling/a-2",
        "scaling/a-4",
        "scaling/a-8",
        "scaling/a-16",
        "scaling/b-1",
        "scaling/b-2",
        "scaling/b-4",
        "scaling/b-8",
        "scaling/b-16",
    })));
