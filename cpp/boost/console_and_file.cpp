#include <stdexcept>
#include <string>
#include <iostream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

enum { LOG_RECORDS_TO_WRITE = 10 };

int main(int argc, char* argv[])
{
    try
    {
        boost::shared_ptr< logging::core > core = logging::core::get();

        // Create a backend and attach a couple of streams to it
        boost::shared_ptr< sinks::text_ostream_backend > backend =
            boost::make_shared< sinks::text_ostream_backend >();

        backend->add_stream(
            boost::shared_ptr< std::ostream >(&std::clog, boost::null_deleter()));

        typedef sinks::synchronous_sink< sinks::text_ostream_backend > stream_sink_t;
        boost::shared_ptr< stream_sink_t > stream_sink(new stream_sink_t(backend));
        core->add_sink(stream_sink);

        // Create a text file sink
        typedef sinks::synchronous_sink< sinks::text_file_backend > file_sink;
        shared_ptr< file_sink > sink(new file_sink(
            keywords::file_name = "%y%m%d.log",      // file name pattern
            keywords::rotation_size = 16384                     // rotation size, in characters
            ));

        sink->set_formatter
        (
            expr::stream
                << "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f") << "]"
                << "[" << expr::attr< attrs::current_thread_id::value_type >("ThreadID") << "] "
                << expr::smessage
        );

        // Add it to the core
        core->add_sink(sink);

        // Add some attributes too
        core->add_global_attribute("TimeStamp", attrs::local_clock());
        core->add_global_attribute("ThreadID", attrs::current_thread_id());

        // Do some logging
        src::logger lg;
        for (unsigned int i = 0; i < LOG_RECORDS_TO_WRITE; ++i) {
            BOOST_LOG(lg) << "Some log record";
        }

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << "FAILURE: " << e.what() << std::endl;
        return 1;
    }
}
