#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

boost::asio::io_service ioservice;
boost::array<char,4096> buffer;

boost::process::pipe in(ioservice);

void end_read(const boost::system::error_code &ec,std::size_t bytes_transferred);

void begin_read()
{
    // Register a handler for the asynchronous read.
    in.async_read_some(
        boost::asio::buffer(buffer)
    ,   boost::bind(&end_read,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
    );
}

void end_read(const boost::system::error_code &ec,std::size_t bytes_transferred)
{
    if (!ec) {
        // If there is no error, process the output and re-register the handler.
        std::cout << std::string(buffer.data(),bytes_transferred) << std::flush;
        begin_read();
    }
}

int main()
{
    std::string exe=boost::process::find_executable_in_path("ping");
    std::vector<std::string> args;
    args.push_back("www.heise.de");

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

    system("pause");
}
