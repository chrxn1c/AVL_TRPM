#pragma once

#include <cstring>
#include <iostream>
#include <fstream>

namespace MovieConstants {
	const unsigned MOVIE_THEATRE_SIZE = 40 + 1;
	const unsigned MOVIE_NAME_SIZE = 40 + 1;
	const unsigned DATE_SIZE = 10 + 1;
	const unsigned BEGINNING_TIME_SIZE = 5 + 1;
	const unsigned TICKET_SIZE = sizeof(double);
}

struct Movie {
	char movieTheatre[MovieConstants::MOVIE_THEATRE_SIZE];
	char movieName[MovieConstants::MOVIE_NAME_SIZE];
	char date[MovieConstants::DATE_SIZE];
	char beginningTime[MovieConstants::BEGINNING_TIME_SIZE];
	double ticketPrice;


	Movie() {}


	Movie(const char movieTheatre[MovieConstants::MOVIE_THEATRE_SIZE],
		const char movieName[MovieConstants::MOVIE_NAME_SIZE],
		const char date[MovieConstants::DATE_SIZE],
		const char beginningTime[MovieConstants::BEGINNING_TIME_SIZE]) {

		strcpy_s(this->movieTheatre, movieTheatre);
		strcpy_s(this->movieName, movieName);
		strcpy_s(this->date, date);
		strcpy_s(this->beginningTime, beginningTime);
	}


	bool operator==(const Movie& target) {
		return !(strcmp(movieTheatre, target.movieTheatre) ||
			strcmp(movieName, target.movieName) ||
			strcmp(date, target.date) ||
			strcmp(beginningTime, target.beginningTime));
	}


	bool operator!=(const Movie& target) {
		return !(this->operator==(target));
	}

	bool operator ==(const char* binaryWord) {
		return (strcmp(movieTheatre, binaryWord) == 0);
	}

	bool operator!=(const char* binaryWord) {
		return !(this->operator==(binaryWord));
	}

	Movie& addTicketPrice(const double& addition) {
		this->ticketPrice += addition;
		return *this;
	}
};

std::ostream& operator<<(std::ostream& cout, const Movie& target) {
	cout
		<< target.movieTheatre << std::endl
		<< target.movieName << std::endl
		<< target.date << std::endl
		<< target.beginningTime << std::endl
		<< target.ticketPrice << std::endl;

	return cout;
}

std::fstream& operator>>(std::fstream& stream, Movie& movie) {
	stream.getline(movie.movieTheatre, MovieConstants::MOVIE_THEATRE_SIZE, '\n');
	stream.getline(movie.movieName, MovieConstants::MOVIE_NAME_SIZE, '\n');
	stream.getline(movie.date, MovieConstants::DATE_SIZE, '\n');
	stream.getline(movie.beginningTime, MovieConstants::BEGINNING_TIME_SIZE, '\n');
	stream >> movie.ticketPrice;

	if (stream.peek() == '\n')
		stream.get();

	return stream;
}

std::fstream& operator<<(std::fstream& stream, const Movie& movie) {
	stream.write((char*)&movie.movieTheatre, MovieConstants::MOVIE_THEATRE_SIZE) << std::endl;
	stream.write((char*)&movie.movieName, MovieConstants::MOVIE_NAME_SIZE) << std::endl;
	stream.write((char*)&movie.date, MovieConstants::DATE_SIZE) << std::endl;
	stream.write((char*)&movie.beginningTime, MovieConstants::BEGINNING_TIME_SIZE) << std::endl;
	stream.write((char*)&movie.ticketPrice, MovieConstants::TICKET_SIZE) << std::endl;
	return stream;
}

