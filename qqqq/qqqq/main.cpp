#include <SFML/Network.hpp>
#include <iostream>
#include <SFML/System.hpp>

struct GameState {
    sf::Vector2f ballPosition;
    sf::Vector2f paddlePosition1;
    sf::Vector2f paddlePosition2;
};

class PongServer {
public:
    PongServer(unsigned short port) : port(port) {}

    void run() {
        sf::TcpListener listener;
        listener.listen(port);

        std::cout << "Server listening on port " << port << std::endl;

        sf::TcpSocket client1, client2;
        listener.accept(client1);
        listener.accept(client2);

        GameState gameState;
        sf::Vector2f ballVelocity(2.0f, 2.0f); // Example velocity

        while (true) {
            // Update ball position based on velocity
            gameState.ballPosition += ballVelocity;

            // Bounce the ball if it reaches the top or bottom of the window
            if (gameState.ballPosition.y < 0 || gameState.ballPosition.y > 600) {
                ballVelocity.y = -ballVelocity.y;
            }

            // Bounce the ball if it reaches the left or right of the window
            if (gameState.ballPosition.x < 0 || gameState.ballPosition.x > 800) {
                ballVelocity.x = -ballVelocity.x;
            }

            // Send game state to both clients
            sf::Packet gameStatePacket;
            gameStatePacket << gameState.ballPosition.x << gameState.ballPosition.y
                << gameState.paddlePosition1.y << gameState.paddlePosition2.y;
            client1.send(gameStatePacket);
            client2.send(gameStatePacket);

            // Receive paddle positions from both clients
            sf::Packet paddlePacket1, paddlePacket2;

            if (client1.receive(paddlePacket1) == sf::Socket::Done) {
                paddlePacket1 >> gameState.paddlePosition1.y;
            }

            if (client2.receive(paddlePacket2) == sf::Socket::Done) {
                paddlePacket2 >> gameState.paddlePosition2.y;
            }

            // Simulate latency (sleep for a certain amount of time)
            sf::sleep(sf::milliseconds(1));  // Adjust the value to set the desired latency

            // ...
        }
    }

private:
    unsigned short port;
};

int main() {
    PongServer server(53000);
    server.run();

    return 0;
}
