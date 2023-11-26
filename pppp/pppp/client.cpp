#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

struct GameState {
    sf::Vector2f ballPosition;
    sf::Vector2f paddlePosition1;
    sf::Vector2f paddlePosition2;
};

class PongClient {
public:
    PongClient() {}

    void run() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Pong Client");

        sf::TcpSocket socket;
        if (socket.connect("127.0.0.1", 53000) != sf::Socket::Done) {
            std::cout << "Failed to connect to the server" << std::endl;
            return;
        }

        GameState gameState;
        sf::Vector2f ballVelocity(2.0f, 2.0f); // Example velocity

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            // Handle user input and update game state
            handleInput(gameState);

            // Send paddle positions to the server
            sf::Packet paddlePacket;
            paddlePacket << gameState.paddlePosition1.y << gameState.paddlePosition2.y;
            socket.send(paddlePacket);

            // Receive updated game state from the server (including ball position)
            sf::Packet gameStatePacket;
            socket.receive(gameStatePacket);
            gameStatePacket >> gameState.ballPosition.x >> gameState.ballPosition.y
                >> gameState.paddlePosition1.y >> gameState.paddlePosition2.y;

            // Update the ball position based on velocity
            gameState.ballPosition += ballVelocity;

            // Bounce the ball if it reaches the top or bottom of the window
            if (gameState.ballPosition.y < 0 || gameState.ballPosition.y > 600) {
                ballVelocity.y = -ballVelocity.y;
            }

            // Bounce the ball if it reaches the left or right of the window
            if (gameState.ballPosition.x < 0 || gameState.ballPosition.x > 800) {
                ballVelocity.x = -ballVelocity.x;
            }

            // Bounce the ball if it hits the paddle
            if (gameState.ballPosition.x < 30 && gameState.ballPosition.y > gameState.paddlePosition1.y &&
                gameState.ballPosition.y < gameState.paddlePosition1.y + 100) {
                ballVelocity.x = -ballVelocity.x;
            }

            if (gameState.ballPosition.x > 770 && gameState.ballPosition.y > gameState.paddlePosition2.y &&
                gameState.ballPosition.y < gameState.paddlePosition2.y + 100) {
                ballVelocity.x = -ballVelocity.x;
            }

            // Clear the window
            window.clear();

            // Draw game objects
            drawObjects(window, gameState);

            // Display the updated window
            window.display();
        }
    }

private:
    sf::RectangleShape paddle{ sf::Vector2f(20, 100) };
    float paddleSpeed = 5.0f;
    sf::CircleShape ball{ 10.0f };

    void handleInput(GameState& gameState) {
        // Handle user input and update paddle positions
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && gameState.paddlePosition1.y > 0) {
            gameState.paddlePosition1.y -= paddleSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && gameState.paddlePosition1.y + paddle.getSize().y < 600) {
            gameState.paddlePosition1.y += paddleSpeed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && gameState.paddlePosition2.y > 0) {
            gameState.paddlePosition2.y -= paddleSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && gameState.paddlePosition2.y + paddle.getSize().y < 600) {
            gameState.paddlePosition2.y += paddleSpeed;
        }
    }

    void drawObjects(sf::RenderWindow& window, const GameState& gameState) {
        // Draw game objects
        paddle.setPosition(10, gameState.paddlePosition1.y);
        window.draw(paddle);

        sf::RectangleShape opponentPaddle(sf::Vector2f(20, 100));
        opponentPaddle.setPosition(770, gameState.paddlePosition2.y);
        window.draw(opponentPaddle);

        ball.setPosition(gameState.ballPosition);
        window.draw(ball);
    }
};

int main() {
    PongClient client;
    client.run();

    return 0;
}
