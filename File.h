#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include "Movie.h"

class File {
private:
	std::fstream initialFile;
	std::fstream binaryFile;
	const std::string initialFilePath;
	std::string binaryFilePath;
	bool exists = true;
public:
	File(const std::string& _initialFilePath = "initialFile.txt") : initialFilePath(_initialFilePath) {
		initialFile.open(initialFilePath);

		if (!initialFile.is_open()) {
			std::cout << "Initial file processing failed due to unknown reason.\n";
			exists = false;
			initialFile.clear();
			return;
		}
	}

	const bool getExists() const { return exists; }

	friend std::fstream& operator>>(File &, Movie& movie);
	friend std::fstream& operator<<(File &, const Movie& movie);

	std::fstream& readInBinaryMode(std::fstream& stream, Movie& movie) {
		stream.read((char*)&movie.movieTheatre, MovieConstants::MOVIE_THEATRE_SIZE).get();
		stream.read((char*)&movie.movieName, MovieConstants::MOVIE_NAME_SIZE).get();
		stream.read((char*)&movie.date, MovieConstants::DATE_SIZE).get();
		stream.read((char*)&movie.beginningTime, MovieConstants::BEGINNING_TIME_SIZE).get();
		stream.read((char*)&movie.ticketPrice, MovieConstants::TICKET_SIZE);

		if (stream.peek() == '\n')
			stream.get();

		return stream;
	}

	int createBinaryFile(const std::string newBinaryFilePath = "newBinaryFile.txt") {
		if (!exists) {
			std::cout << "Initial file does not exist. Therefore, cannot create a binary file.\n";
			return 0;
		}

		if (binaryFile.is_open()) {
			std::cout << "Binary file is already open.\n";
			return 0;
		}
		initialFile.clear();
		initialFile.seekg(0);
		binaryFilePath = newBinaryFilePath;
		binaryFile.open(newBinaryFilePath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

		if (!binaryFile.is_open()) {
			std::cout << "Binary file processing failed due to unknown reason.\n";
			exists = false;
			return 0;
		}

		Movie content;

		while (!initialFile.eof()) {
			initialFile >> content;
			if (content == "BINARY")
				break;
			binaryFile << content;
		}


		initialFile.clear();
		binaryFile.clear();
		return 1;
	}


	int initialSelfBackup() {
		if (!exists) {
			std::cout << "File with chosen filepath doesn't exist so far.\n";
			return 0;
		}

		if (!binaryFile.is_open()) {
			std::cout << "Binary file is not open. Cannot make a deep copy.\n";
			return 0;
		}

		bool isOverwriting = false;
		std::string content;
		initialFile.seekg(0);
		binaryFile.seekg(0);
		std::fstream auxiliary("auxiliary.txt", std::ios_base::trunc | std::ios_base::out | std::ios_base::in);


		while (std::getline(initialFile, content)) {
			if (content == "BINARY") {
				isOverwriting = true;
				break;
			}
			else
				auxiliary << content << '\n';
		}

		auxiliary << "BINARY\n";

		while (binaryFile.peek() != -1) {
			std::getline(binaryFile, content);
			auxiliary << content << "\n";
		}

		initialFile.close();
		auxiliary.clear();
		auxiliary.seekg(0);
		binaryFile.clear();

		initialFile.open(initialFilePath, std::ios_base::out | std::ios_base::trunc);

		isOverwriting ? std::cout << "Overwriting the contents\n" : std::cout << "Copying the contents of binary file\n";

		while (std::getline(auxiliary, content))
			initialFile << content << "\n";

		initialFile.close();
		initialFile.open(initialFilePath);
		binaryFile.clear();

		return 1;
	}


	int binaryFileDisplay() {
		if (!exists) {
			std::cout << "File with chosen filepath doesn't exist so far.\n";
			return 0;
		}

		if (!binaryFile.is_open()) {
			std::cout << "Binary file is not open. Cannot make a deep copy.\n";
			return 0;
		}

		binaryFile.seekg(0);

		Movie content{};
		std::cout << "\n\n==========================" << std::endl;

		/*.peek() == -1 stands for "the next symbol is the '\n', left by std::endl when writing a latter double, meaning the end of the file.
		binaryFile.eof() doensn't work because of this left '\n'.*/

		while (binaryFile.peek() != -1) {
			readInBinaryMode(binaryFile, content);
			std::cout << content << " " << std::endl;
		}
		std::cout << "==========================\n\n" << std::endl;

		binaryFile.clear();
		return 1;
	}


	Movie readMovie(const int& index) {
		if (!binaryFile.is_open()) {
			std::cout << "\nCannot read the given movie, binary file is not open.\n";
			return Movie{};
		}

		if (index < 0)
			throw std::invalid_argument("Received negative value\n");

		binaryFile.seekg(0);
		Movie movie{};

		for (int i = 0; i <= index; i++)
			if (!readInBinaryMode(binaryFile, movie)) {
				std::cout << "Your index is out of boundaries: file contains few movies.\n";
				binaryFile.clear();
				return Movie{};
			}

		binaryFile.clear();
		return movie;
	}


	int addMovie(const Movie & movie) {

		if (!binaryFile.is_open()) {
			std::cout << "Binary file is not open. Cannot delete the chosen movie.\n";
			return 0;
		}

		binaryFile.seekg(std::ios_base::end);
		binaryFile << movie;

		binaryFile.clear();
	}

	int deleteMovie(const char movieTheatre[MovieConstants::MOVIE_THEATRE_SIZE],
		const char movieName[MovieConstants::MOVIE_NAME_SIZE],
		const char date[MovieConstants::DATE_SIZE],
		const char beginningTime[MovieConstants::BEGINNING_TIME_SIZE]) {

		if (!binaryFile.is_open()) {
			std::cout << "Binary file is not open. Cannot delete the chosen movie.\n";
			return 0;
		}

		std::fstream auxiliary("auxiliary.txt", std::ios_base::trunc | std::ios_base::out);
		Movie content{}, toDelete(movieTheatre, movieName, date, beginningTime), last{};
		bool isToDeleteFound = false;

		//why sizeof() + 5? no idea, it was just found out.
		binaryFile.seekg(-(int)(sizeof(Movie) + 5), std::ios_base::end);
		readInBinaryMode(binaryFile, last);

		binaryFile.clear();
		binaryFile.seekg(0);

		while (readInBinaryMode(binaryFile, content)) {
			if (content == toDelete) {
				isToDeleteFound = true;

				if (toDelete == last)
					break;
				else
					auxiliary << last;
			}

			else {
				if (content == last && isToDeleteFound)
					break;
				else
					auxiliary << content;
			}
		}

		isToDeleteFound ? std::cout << "Found a movie to delete.\n" : std::cout << "Such a movie hasn't been found, no changes will be present.\n";

		binaryFile.close();
		binaryFile.open(binaryFilePath, std::ios_base::out | std::ios_base::trunc);

		auxiliary.close();
		auxiliary.open("auxiliary.txt", std::ios_base::in);

		while (readInBinaryMode(auxiliary, content))
			binaryFile << content;

		binaryFile.close();
		auxiliary.close();
		binaryFile.open(binaryFilePath);

		return 1;
	}


	int increasePrice(const char movieTheatre[MovieConstants::MOVIE_THEATRE_SIZE],
		const char movieName[MovieConstants::MOVIE_NAME_SIZE],
		const char date[MovieConstants::DATE_SIZE],
		const char beginningTime[MovieConstants::BEGINNING_TIME_SIZE],
		double addition) {

		if (!binaryFile.is_open()) {
			std::cout << "Binary file is not open. Cannot delete the chosen movie.\n";
			return 0;
		}
		binaryFile.seekg(0);

		std::fstream auxiliary("auxiliary.txt", std::ios_base::trunc | std::ios_base::out);
		bool isFound = false;
		Movie content{}, required(movieTheatre, movieName, date, beginningTime);


		while (readInBinaryMode(binaryFile, content))
			if (content == required) {
				isFound = true;
				auxiliary << content.addTicketPrice(addition);
			}
			else
				auxiliary << content;

		if (isFound)
			std::cout << "Increasing the chosen movie's price.\n";
		else
			std::cout << "Such a movie hasn't been found.\n";

		binaryFile.close();
		binaryFile.open(binaryFilePath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

		auxiliary.close();
		auxiliary.open("auxiliary.txt", std::ios_base::in);

		while (readInBinaryMode(auxiliary, content))
			binaryFile << content;

		binaryFile.clear();
		auxiliary.close();

		return 1;
	}


	int formMovieList(const char targetDate[MovieConstants::DATE_SIZE]) {

		if (!binaryFile.is_open()) {
			std::cout << "\nCannot read the given movie, binary file is not open.\n";
			return 0;
		}

		binaryFile.seekg(0);
		std::ofstream newFile(std::string(targetDate) + ".txt", std::ios_base::out | std::ios_base::trunc);

		Movie content{};

		while (readInBinaryMode(binaryFile, content))
			if (strcmp(targetDate, content.date) == 0)
				newFile << content;

		binaryFile.clear();
		newFile.close();

		return 1;
	}


	~File() {
		if (!initialFile.good())
			std::cout << "File with filepath = " << initialFilePath << "has some failbits set to 1. ";
		if (!binaryFile.good())
			std::cout << "File with filepath = " << binaryFilePath << "has some failbits set to 1. ";

		initialFile.close();
		binaryFile.close();
	}
};


std::fstream& operator>>(File& file, Movie& movie) {
	file.initialFile >> movie;

	return file.initialFile;
}


std::fstream& operator<<(File& file, const Movie& movie) {
	file.binaryFile.write((char*)&movie.movieTheatre, MovieConstants::MOVIE_THEATRE_SIZE) << std::endl;
	file.binaryFile.write((char*)&movie.movieName, MovieConstants::MOVIE_NAME_SIZE) << std::endl;
	file.binaryFile.write((char*)&movie.date, MovieConstants::DATE_SIZE) << std::endl;
	file.binaryFile.write((char*)&movie.beginningTime, MovieConstants::BEGINNING_TIME_SIZE) << std::endl;
	file.binaryFile.write((char*)&movie.ticketPrice, MovieConstants::TICKET_SIZE) << std::endl;

	return file.binaryFile;
}