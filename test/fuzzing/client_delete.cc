#include <stdint.h>
#include <stddef.h>

#include <httplib.h>
#include <thread>

#define PORT 8080

const char *HOST = "localhost";
const char *response = "Hello World";

using namespace std;
using namespace httplib;

std::thread ts;
Server svr;

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv) {
	svr.Delete(R"(.*)", [&](const Request & /*req*/, Response &res) {
		res.set_content(response, "text/plain");
	});

	ts = std::thread([&]() { svr.listen(HOST, PORT); });
	return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	if (size > 0) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		{
			Client cli(HOST, PORT);
			auto res = cli.Delete(reinterpret_cast<const char *>(data));
		}
	}
	svr.stop();
	return 0;
}
