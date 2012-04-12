#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

class ProcessCapture
{
  public:

    typedef std::vector<std::string> Arguments;

    ProcessCapture()
    :   m_in(m_ioservice)
    {
    }

    void run(const std::string& exe,const std::string& arg="") {
        Arguments args;
        boost::split(args,arg,boost::is_space(),boost::token_compress_on);

        run_process(find_executable(exe),args);
    }

  private:

    static std::string find_executable(std::string exe) {
        boost::system::error_code ec;

        if (!boost::filesystem::exists(exe,ec)) {
            try {
                exe=boost::process::find_executable_in_path(exe);
            }
            catch (...) {
                exe.clear();
            }
        }

        return exe;
    }

    void run_process(const std::string& exe,const Arguments& args) {
        if (exe.empty()) {
            return;
        }

        // Replace STDOUT in the child process with an asynchronous pipe.
        boost::process::context ctx;
        ctx.streams[boost::process::stdout_id]=boost::process::behavior::async_pipe();

        // Launch the child process.
        boost::process::child c=boost::process::create_child(exe,args,ctx);

        // Assign the child's STDOUT handle to our pipe.
        boost::process::handle h=c.get_handle(boost::process::stdout_id);
        m_in.assign(h.release());

        begin_read();

        m_ioservice.run();
    }

    void begin_read();

    void end_read(const boost::system::error_code &ec,std::size_t bytes_transferred) {
        if (!ec) {
            // If there is no error, process the output and re-register the handler.
            std::cout << std::string(m_buffer.data(),bytes_transferred) << std::flush;
            begin_read();
        }
    }

    boost::asio::io_service m_ioservice;
    boost::array<char,4096> m_buffer;

    boost::process::pipe m_in;
};

inline void ProcessCapture::begin_read()
{
    // Register a handler for the asynchronous read.
    m_in.async_read_some(
        boost::asio::buffer(m_buffer)
    ,   boost::bind(&ProcessCapture::end_read,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
    );
}
