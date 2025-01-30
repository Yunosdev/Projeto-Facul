#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>

// inimigos e projeteis
struct Projetil {
    sf::RectangleShape shape;
    sf::Vector2f direction;
    float speed;
};

struct Inimigo {
    sf::CircleShape shape;
    float speed;
};

int main() {
    srand(time(0));
    
    sf::RenderWindow window(sf::VideoMode(800, 600), "Piu Piu Bang");
    window.setFramerateLimit(60);
    
    bool jogoIniciado = false;
    sf::CircleShape bolaInicio(10);
    bolaInicio.setFillColor(sf::Color::White);
    sf::Vector2f bolaVelocidade(3, 3);
    bolaInicio.setPosition(400, 300);
    
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text startText("Pressione Enter para Iniciar", font, 24);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(250, 280);
    
    // boneco
    sf::CircleShape boneco(20);
    boneco.setFillColor(sf::Color::Blue);
    boneco.setPosition(400, 300);
    
    // arma
    sf::RectangleShape arma(sf::Vector2f(30, 5));
    arma.setFillColor(sf::Color::Black);
    arma.setOrigin(0, 2.5f);
    
    std::vector<Projetil> projeteis;
    std::vector<Inimigo> inimigos;
    
    sf::Clock inimigoTimer;
    float inimigoSpeed = 2.0f;
    bool vivo = true;
    int score = 0;
    
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    // som de tiro
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("tiro.wav")) {
        return -1; // Erro ao carregar
    }
    sf::Sound sound;
    sound.setBuffer(buffer);

    // som de vitória
    sf::SoundBuffer victoryBuffer;
    if (!victoryBuffer.loadFromFile("vitoria.wav")) {
        return -1; // Erro ao carregar
    }
    sf::Sound victorySound;
    victorySound.setBuffer(victoryBuffer);

    // som ao matar inimigo
    sf::SoundBuffer killBuffer;
    if (!killBuffer.loadFromFile("mato.wav")) {
        return -1; // Erro ao carregar
    }
    sf::Sound killSound;
    killSound.setBuffer(killBuffer);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (!jogoIniciado && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                jogoIniciado = true;
            if (!vivo && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                vivo = true;
                score = 0;
                inimigoSpeed = 2.0f;
                projeteis.clear();
                inimigos.clear();
                boneco.setPosition(390, 280);
            }
        }
        
        window.clear();
        
        if (!jogoIniciado) {
            bolaInicio.move(bolaVelocidade);
            if (bolaInicio.getPosition().x <= 0 || bolaInicio.getPosition().x >= 790)
                bolaVelocidade.x *= -1;
            if (bolaInicio.getPosition().y <= 0 || bolaInicio.getPosition().y >= 590)
                bolaVelocidade.y *= -1;
            
            window.draw(startText);
            window.draw(bolaInicio);
        } else {
            if (vivo) {
                sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
                sf::Vector2f center = boneco.getPosition() + sf::Vector2f(20, 20);
                sf::Vector2f direction = mousePos - center;
                float angle = atan2(direction.y, direction.x) * 180 / 3.14159f;
                arma.setPosition(center);
                arma.setRotation(angle);
                
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    Projetil p;
                    p.shape.setSize(sf::Vector2f(10, 4));
                    p.shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
                    p.shape.setPosition(center);
                    p.direction = direction / sqrt(direction.x * direction.x + direction.y * direction.y);
                    p.speed = 6.0f;
                    projeteis.push_back(p);
                    sound.play(); // Tocar som de tiro
                }
                
                if (inimigoTimer.getElapsedTime().asSeconds() > 1.0f) {
                    Inimigo i;
                    i.shape.setRadius(15);
                    i.speed = inimigoSpeed;
                    i.shape.setPosition(rand() % 800, rand() % 600);
                    if (i.speed < 3.0f)
                        i.shape.setFillColor(sf::Color::Green);
                    else if (i.speed < 5.0f)
                        i.shape.setFillColor(sf::Color::Yellow);
                    else
                        i.shape.setFillColor(sf::Color::Red);
                    inimigos.push_back(i);
                    inimigoTimer.restart();
                }
                
                for (size_t i = 0; i < projeteis.size(); i++) {
                    projeteis[i].shape.move(projeteis[i].direction * projeteis[i].speed);
                    if (projeteis[i].shape.getPosition().x < 0 || projeteis[i].shape.getPosition().x > 800 || 
                        projeteis[i].shape.getPosition().y < 0 || projeteis[i].shape.getPosition().y > 600) {
                        projeteis.erase(projeteis.begin() + i);
                    }
                }
                
                for (size_t i = 0; i < inimigos.size(); i++) {
                    sf::Vector2f dir = center - inimigos[i].shape.getPosition();
                    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
                    dir /= length;
                    inimigos[i].shape.move(dir * inimigos[i].speed);
                    if (inimigos[i].shape.getGlobalBounds().intersects(boneco.getGlobalBounds())) {
                        vivo = false;
                        if (score > 100) {
                            victorySound.play();
                        }
                    }
                }
                
                for (size_t i = 0; i < projeteis.size(); i++) {
                    for (size_t j = 0; j < inimigos.size(); j++) {
                        if (projeteis[i].shape.getGlobalBounds().intersects(inimigos[j].shape.getGlobalBounds())) {
                            projeteis.erase(projeteis.begin() + i);
                            inimigos.erase(inimigos.begin() + j);
                            score++;
                            inimigoSpeed += 0.1f;
                            killSound.play();
                            break;
                        }
                    }
                }
                
                std::stringstream ss;
                ss << "Score: " << score;
                scoreText.setString(ss.str());
                
                window.draw(boneco);
                window.draw(arma);
                for (auto& p : projeteis)
                    window.draw(p.shape);
                for (auto& i : inimigos)
                    window.draw(i.shape);
                window.draw(scoreText);
            } else if (score == 69) {
                sf::Text text("Criado por YunoDev, projeto de faculdade", font, 24);
                text.setFillColor(sf::Color::White);
                text.setPosition(150, 280);
                window.draw(text);
            } else {
                sf::Text gameOverText("Game Over! Pressione Enter para reiniciar", font, 24);
                gameOverText.setFillColor(sf::Color::White);
                gameOverText.setPosition(200, 280);
                window.draw(gameOverText);
            }
        }
        window.display();
    }
    
    return 0;
}