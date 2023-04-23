#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include <windows.h>
#include <cstdlib>
#include <ctime>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1040

#define JAB_WIDTH 40
#define JAB_HEIGHT 40

#define MOVE 40     //za koliko ce se segment zmije pomeriti
#define DELAY 60    //koliko je kasnjenje do sledeceg pomeraja

#define SNAKE_WIDTH 40
#define SNAKE_HEIGHT 40



sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH,WINDOW_HEIGHT),"SNAKE GAME");
sf::RectangleShape jabuka(sf::Vector2f (JAB_WIDTH,JAB_HEIGHT));
std::vector<sf::RectangleShape> snake(2);
std::vector<sf::Vector2f>koordinate_pokreta(1);
sf::Clock clock1;

sf::SoundBuffer game_over_buff;
sf::SoundBuffer eating_buff;
sf::Sound death_sound;
sf::Sound eating_sound;

bool isDead=true;
int score=0;

void resetuj_zmiju();
void generisi_jabuku();
void pomeraj_glave(char c);
void pomeraj_tela();
void generisi_skretanje();


bool zmija_jede_jabuku();
bool zmija_jede_sebe();
void zmija_raste();


int main() {

                                                    //TXT + SOUND
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    sf::SoundBuffer music_buff;
    sf::Sound music_sound;

    if(!music_buff.loadFromFile("C:\\Users\\pc\\Desktop\\programiranje\\Snake_game\\music\\Retro.wav"))
    {
        std::cout<<"Greska pri ucitavanju muzike\n";
        system("pause");
    }
    music_sound.setBuffer(music_buff);
    music_sound.setLoop(true);
    music_sound.play();

    if(!game_over_buff.loadFromFile("C:\\Users\\pc\\Desktop\\programiranje\\Snake_game\\music\\Game-over.wav"))
    {
        std::cout<<"Greska pri ucitavanju game over sounda\n";
        system("pause");
    }

    death_sound.setBuffer(game_over_buff);

    if(!eating_buff.loadFromFile("C:\\Users\\pc\\Desktop\\programiranje\\Snake_game\\music\\Eating_SE.wav"))
    {
        std::cout<<"Greska pri ucitavanju eating sounda\n";
        system("pause");
    }

    eating_sound.setBuffer(eating_buff);

    sf::Font font;

    if(!font.loadFromFile("C:\\Users\\pc\\Desktop\\programiranje\\Snake_game\\fonts\\Golden Age Shad.ttf"))
    {
        std::cout<<"Greska pri ucitavanju fonta\n";
        system("pause");
    }

    sf::Text txtGameName;
    sf::Text txtScreen;
    sf::Text txtScore;

    txtScreen.setFont(font);
    txtGameName.setFont(font);
    txtScore.setFont(font);

    txtScreen.setString("PRESS SPACE TO PLAY");

    txtScreen.setCharacterSize(128);
    txtScreen.setFillColor(sf::Color::Green);
    txtScreen.setPosition(180,100);
    txtScreen.setStyle(sf::Text::Style::Bold);
    txtGameName.setString("SNAKE GAME");
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






    srand(time(NULL));
    snake.at(0).setSize(sf::Vector2f (SNAKE_WIDTH,SNAKE_HEIGHT));
    snake.at(1).setSize(sf::Vector2f (SNAKE_WIDTH,SNAKE_HEIGHT));
    resetuj_zmiju();    //reset da bih im postavio koordinate na pocetak (snake[1] i snake[0])
    snake.at(0).setFillColor(sf::Color::Yellow);
    snake.at(1).setFillColor(sf::Color::Green);
    koordinate_pokreta.at(0)=snake.at(0).getPosition();
    jabuka.setFillColor(sf::Color::Red);
    generisi_jabuku();

    while (window.isOpen())
    {
        sf::Event event;
        char c;

        while (window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
                window.close();
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                if(isDead== true)
                {
                    score=0;
                    c=' ';  //da bi igrac mogao sam da odredi pocetni pravac
                }

                isDead=false;
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
                window.close();
            else
            {
                if((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) )&& c!='d' && isDead==false)
                {
                    if(snake[0].getPosition().x<=snake[1].getPosition().x)  //dodatne provere izvrsavam da zmija ne bi prosla kroz sebe pri brzom unosu korisnika
                       c='a';
                }
                else if((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) && c!='s'&& isDead==false)
                {
                    if(snake[0].getPosition().y>=snake[1].getPosition().y)
                        c='w';
                }
                else if((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))  && c!='a'&& isDead==false)
                {
                    if(snake[0].getPosition().x>=snake[1].getPosition().x)
                         c='d';
                }
                else if((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) && c!='w'&& isDead==false)
                {
                    if(snake[0].getPosition().y<=snake[1].getPosition().y)
                        c='s';
                }
            }
        }


        if(isDead== false)
        {
            pomeraj_glave(c);
            window.draw(jabuka);
            window.draw(txtGameName);
            txtScore.setCharacterSize(40);
            txtScore.setString(std::to_string(score));
            txtScore.setPosition(1800,0);
            window.draw(txtScore);
        }

        else
        {
            window.clear();
            txtScore.setCharacterSize(64);
            txtScore.setString("LAST SCORE :");
            txtScore.setPosition(630,600);
            window.draw(txtScreen);
            window.draw(txtScore);

            txtScore.setString(std::to_string(score));
            txtScore.setPosition(1130,600);
            window.draw(txtScore);

           resetuj_zmiju();
           window.draw(snake.at(0));
           window.draw(snake.at(1));
        }

        window.display();
    }


    return 0;

}

void resetuj_zmiju()
{
    snake.resize(2);
    snake.at(0).setPosition(WINDOW_WIDTH/2-SNAKE_WIDTH, WINDOW_HEIGHT/2-SNAKE_HEIGHT);
    snake.at(1).setPosition(WINDOW_WIDTH/2-SNAKE_WIDTH, WINDOW_HEIGHT/2-SNAKE_HEIGHT);

}

void generisi_jabuku()
{
    int x,y;
    bool jabuka_sece_zmiju=true;
    sf::RectangleShape rect(sf::Vector2f(JAB_WIDTH,JAB_HEIGHT));
    do{
        //unutar rand delim sa sirinom/visinom pa posle mnozim da bih dobio brojeve deljive sa sirinom jabuke/zmijine glave
        x=rand()% ((WINDOW_WIDTH-JAB_WIDTH)/JAB_WIDTH);
        y=rand()% ((WINDOW_HEIGHT-JAB_HEIGHT)/JAB_HEIGHT);
        x*=JAB_WIDTH;
        y*=JAB_HEIGHT;
        rect.setPosition(x,y);
        jabuka_sece_zmiju=false;
        for(int i=0;i<snake.size();i++)
            if(snake[i].getGlobalBounds().intersects(rect.getGlobalBounds()))
            {
                jabuka_sece_zmiju= true;
                break;
            }

    }while(jabuka_sece_zmiju);

    jabuka.setPosition(x,y);

    jabuka.setPosition(x,y);
    window.draw(jabuka);

}
void pomeraj_glave(char c)
{
   //Sleep(DELAY) pravi manje smooth kretanje od sledece 2 linije
    while (clock1.getElapsedTime().asMilliseconds() < DELAY) {}
    clock1.restart();

    switch (c) {
        case 'a':
            snake.at(0).move(sf::Vector2f(-MOVE,0.f));

            if(snake.at(0).getPosition().x<0-20)
            {
                death_sound.play();
                isDead= true;
            }
            break;

        case 'w':

            snake.at(0).move(sf::Vector2f(0.f,-MOVE));

            if(snake.at(0).getPosition().y<0)
            {
                death_sound.play();
                isDead= true;
            }
            break;

        case 's':
            snake.at(0).move(sf::Vector2f(0.f,MOVE));

            if((snake.at(0).getPosition().y+SNAKE_HEIGHT)>WINDOW_HEIGHT+20)
            {
                death_sound.play();
                isDead= true;
            }
            break;

        case 'd':
            snake.at(0).move(sf::Vector2f(MOVE,0.f));

            if((snake.at(0).getPosition().x+SNAKE_WIDTH)>WINDOW_WIDTH+20)
            {
                death_sound.play();
                isDead= true;
            }
            break;

    }

    pomeraj_tela();

    if(zmija_jede_sebe()== true)
    {
        death_sound.play();

        isDead= true;
    }
    else if(zmija_jede_jabuku()==true )
       {
           window.clear();
           for(int i=0;i<snake.size()-2;i++)
               window.draw(snake.at(i));

       }
    else
       {
           window.clear();
           for(int i=0;i<snake.size();i++)
               window.draw(snake.at(i));
       }

}

void pomeraj_tela()
{
//pocev od kraja repa svaki deo prati prethodnu poziciju sledeceg segmenta zmije
    for(int i=snake.size()-1;i>0;i--)
    {
        koordinate_pokreta[i] = koordinate_pokreta[i-1];
        snake[i].setPosition(koordinate_pokreta[i]);
    }
    koordinate_pokreta[0] = snake[0].getPosition();

}

void generisi_skretanje()
{
    koordinate_pokreta.push_back(sf::Vector2f(snake.at(0).getPosition().x,snake.at(0).getPosition().y));
}


bool zmija_jede_jabuku()
{
    if (snake[0].getGlobalBounds().intersects(jabuka.getGlobalBounds()))
    {
        eating_sound.play();

        generisi_jabuku();

        zmija_raste();

        generisi_skretanje();

        score+=5;


        return true;

    }
    return false;
}

void zmija_raste()
{

    snake.push_back(sf::RectangleShape(sf::Vector2f(SNAKE_WIDTH, SNAKE_HEIGHT)));
    snake.at(snake.size() - 1).setFillColor(sf::Color::Green);

}

bool zmija_jede_sebe()
{
    for(int i=2;i<snake.size();i++)
        if(snake[0].getGlobalBounds().intersects(snake[i].getGlobalBounds()))
            return true;
    return false;
}
