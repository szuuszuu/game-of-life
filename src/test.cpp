#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char* argv[]) {

    std::vector<std::vector<int>> arr(10, std::vector<int> (10, 0));

    std::string fileName = "data.txt";
    std::string path = "../" + fileName;
    std::string line;
	std::ifstream myFileStream(path);

	if (!myFileStream) {
        std::cerr<<"Error: Missing file named "<<fileName<<std::endl;
		exit(EXIT_SUCCESS);
	}

	while (getline(myFileStream, line)) {
        std::istringstream iss(line);
        int x, y;
        if (!(iss >> x >> y)) {
            std::cerr<<"Error: Invalid line format: "<<line<<std::endl;
            continue;
        }

		if (x < 10 && y < 10) {
			arr[x][y] = 1;
		} else {
            std::cerr<<"Error: Point out of reach: "<<line<<std::endl;
        }
    }

	myFileStream.close();

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            std::cout<<arr[x][y]<<" ";
        }
        std::cout<<std::endl;
    }

}