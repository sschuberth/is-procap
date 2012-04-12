#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

class ProcessCapture
{
  public:

    typedef std::vector<std::string> Arguments;

    ProcessCapture(const std::string& exe,const std::string& arg="")
    :   exe(find_executable(exe))
    ,   in(ioservice)
    {
        boost::split(args,arg,boost::is_space(),boost::token_compress_on);
    }

    ProcessCapture(const std::string& exe,const Arguments& args)
    :   exe(find_executable(exe))
    ,   args(args)
    ,   in(ioservice)
    {
    }

    void operator()() {
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
        in.assign(h.release());

        begin_read();

        ioservice.run();
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

    void begin_read();

    void end_read(const boost::system::error_code &ec,std::size_t bytes_transferred) {
        if (!ec) {
            // If there is no error, process the output and re-register the handler.
            std::cout << std::string(buffer.data(),bytes_transferred) << std::flush;
            begin_read();
        }
    }

    std::string exe;
    Arguments args;

    boost::asio::io_service ioservice;
    boost::array<char,4096> buffer;

    boost::process::pipe in;
};

inline void ProcessCapture::begin_read()
{
    // Register a handler for the asynchronous read.
    in.async_read_some(
        boost::asio::buffer(buffer)
    ,   boost::bind(&ProcessCapture::end_read,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
    );
}
