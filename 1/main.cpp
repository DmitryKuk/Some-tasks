#include <iostream>
#include <string>
#include <vector>
#include <functional>


int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " N" << std::endl;
		return 1;
	}
	
	
	const unsigned int N = std::stoi(argv[1]);
	std::vector<std::pair<unsigned int, unsigned int>> numbers{N + 1};	// Pairs of numbers factors
	std::vector<unsigned int> primes;
	
	
	
	const std::function<void (unsigned int)> print_factors =
		[&](unsigned int i)
		{
			const auto &num = numbers[i];
			std::cout << num.first;
			if (num.second != 1) {
				std::cout << " * ";
				return print_factors(num.second);
			}
		};
	
	
	
	for (unsigned int i = 2; i <= N; ++i) {	// For each number
		// Check current number for prime
		{
			auto &num = numbers[i];
			if (num.first == 0) {
				num.first = i;
				num.second = 1;
				primes.push_back(i);
			}
		}
		
		
		// Mark all numbers like "i * <any prime>" as not prime
		for (auto prime: primes) {
			for (unsigned long mult = i; mult * prime <= N; mult *= prime) {
				auto &num = numbers[mult * prime];
				if (num.first == 0) {	// Number was not processed yet
					num.first = prime;
					num.second = static_cast<unsigned int>(mult);
				}
			}
		}
	}
	
	
	std::cout << "Primes:" << std::endl;
	for (auto prime: primes)
		std::cout << ' ' << prime;
	std::cout << std::endl << std::endl;
	
	
	std::cout << "Factors:" << std::endl;
	for (unsigned int i = 2; i <= N; ++i) {
		std::cout << i << " = ";
		print_factors(i);
		std::cout << std::endl;
	}
	
	
	return 0;
}
