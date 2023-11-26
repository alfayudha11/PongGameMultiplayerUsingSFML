#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "client.h"

client client_;
void clientInit()
{
	//server_.Init();
	client_.Init();
}
int main()
{
	std::cout << "Server is starting \n\n";

	clientInit();

	//server_.Update();
	//client_.Update();
}
