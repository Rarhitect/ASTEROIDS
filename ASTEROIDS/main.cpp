//
// Created on 18.04.2021
// by Alyaev Roman
//
// ASTEROIDS
//

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <time.h>
#include <list>
#include <math.h>
#include <iostream>
#include <random>
#include <memory>
#include <chrono>
#include <thread>


using namespace sf;

const int W = 1200;
const int H = 800;

const float DEGTORAD = 0.017453f;
bool close = false;


class Animation
{
public:
  float Frame, speed;
  Sprite sprite;
  std::vector<IntRect> frames;

public:
  Animation() {}

  Animation(Texture &t, int x, int y, int w, int h, int count, float Speed)
  {
    Frame = 0.0;
    speed = Speed;

    for (int i = 0; i < count; i++)
      frames.push_back(IntRect(x + i * w, y, w, h));

    sprite.setTexture(t);
    sprite.setOrigin(w / 2, h / 2);
    sprite.setTextureRect(frames[0]);
  }
    
    Animation(Texture &t, int x, int y, int w, int h, int count, float Speed, float X, float Y)
    {
      Frame = 0.0;
      speed = Speed;

      for (int i = 0; i < count; i++)
        frames.push_back(IntRect(x + i * w, y, w, h));

      sprite.setTexture(t);
      sprite.setOrigin(w / 2, h / 2);
      sprite.setTextureRect(frames[0]);
        sprite.setScale(X, Y);
    }

  void update()
  {
    Frame += speed;
    int n = frames.size();
    if (Frame >= n)
      Frame -= n;
    if (n > 0)
      sprite.setTextureRect(frames[int(Frame)]);
  }

  bool isEnd()
  {
    return Frame + speed >= frames.size();
  }

  void set_speed(float speed_)
  {
    speed = speed_;
  }
  float get_speed()
  {
    return speed;
  }

};

class Entity
{
private:
    
  float x, y, dx, dy, R, angle;
  int life;
  std::string name;
  

public:
    
Animation anim;
  Entity()
  {
    life = 1;
  }

  void set_x(float x_)
  {
    x = x_;
  }
    
  float get_x()
  {
    return x;
  }

  void set_dx(float dx_)
  {
    dx = dx_;
  }
    
  float get_dx()
  {
    return dx;
  }

  void set_y(float y_)
  {
    y = y_;
  }
    
  float get_y()
  {
    return y;
  }

  void set_dy(float dy_)
  {
    dy = dy_;
  }
    
  float get_dy()
  {
    return dy;
  }

  void set_R(float R_)
  {
    R = R_;
  }
    
  float get_R()
  {
    return R;
  }

  void set_angle(float angle_)
  {
    angle = angle_;
  }
    
  float get_angle()
  {
    return angle;
  }

  void set_life(int life_)
  {
    life = life_;
  }
    
  float get_life()
  {
    return life;
  }

  void set_name(std::string name_)
  {
    name = name_;
  }
    
  std::string get_name()
  {
      return name;
  }

  void set_anim(Animation anim_)
  {
    anim = anim_;
  }
    
  Animation get_anim()
  {
    return anim;
  }

  void add_life(int life_)
  {
    life += life_;
  }
    
  void sub_life(int life_)
  {
    life -= life_;
  }


  void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1)
  {
    anim = a;
    x = X;
    y = Y;
    angle = Angle;
    R = radius;
  }

  virtual void update(){};

  void draw(RenderWindow &app)
  {
    anim.sprite.setPosition(x,y);
    anim.sprite.setRotation(angle+90);
    app.draw(anim.sprite);

    CircleShape circle(R);
    circle.setFillColor(Color(255, 0, 0, 170));
    circle.setPosition(x, y);
    circle.setOrigin(R, R);
  }

  virtual ~Entity(){};
};

class asteroid : public Entity
{
public:
  asteroid()
  {
    std::random_device rd;
    std::mt19937 mersenne(rd());
    std::uniform_real_distribution<> un_distrib(-4, 4);
    
    this->set_dx(un_distrib(mersenne));
    this->set_dy(un_distrib(mersenne));
    set_name("asteroid");
  }

public:
  void update()
  {
    this->set_x(this->get_x() + this->get_dx());
    this->set_y(this->get_y() + this->get_dy());

    if (this->get_x() > W)
      this->set_x(0);
    if (this->get_x() < 0)
      this->set_x(W);
    if (this->get_y() > H)
      this->set_y(0);
    if (this->get_y() < 0)
      this->set_y(H);
  }
};

class bullet : public Entity
{
public:
  bullet()
  {
    this->set_name("bullet");
  }

  void update()
  {
    this->set_dx(cos(this->get_angle() * DEGTORAD) * 6);
    this->set_dy(sin(this->get_angle() * DEGTORAD) * 6);
    this->set_x(this->get_x() + this->get_dx());
    this->set_y(this->get_y() + this->get_dy());

    if (this->get_x() > W || this->get_x() < 0 || this->get_y() > H || this->get_y() < 0)
      this->set_life(0);
  }
};


class healer : public Entity
{
public:
  healer()
  {
    std::random_device rd;
    std::mt19937 mersenne(rd());
    std::uniform_real_distribution<> un_distrib(-4, 4);
    
    this->set_dx(un_distrib(mersenne));
    this->set_dy(un_distrib(mersenne));
    set_name("healer");
  }

public:
  void update()
  {
    this->set_x(this->get_x() + this->get_dx());
    this->set_y(this->get_y() + this->get_dy());

    if (this->get_x() > W)
      this->set_x(0);
    if (this->get_x() < 0)
      this->set_x(W);
    if (this->get_y() > H)
      this->set_y(0);
    if (this->get_y() < 0)
      this->set_y(H);
  }
};



class player : public Entity
{
private:
  bool thrust;
  int scores;
public:
  player()
  {
    this->set_name("player");
    scores = 0;
    this->set_life(3);
  }

  void set_thrust(bool thrust_)
  {
    thrust = thrust_;
  }
  bool get_thrust()
  {
    return thrust;
  }
  void set_scores(int scores_)
  {
    scores = scores_;
  }
  void add_scores(int scores_)
  {
    scores += scores_;
  }
  int get_scores()
  {
    return scores;
  }
  

  void update()
  {
    if (thrust)
    {
      this->set_dx(this->get_dx() + cos(this->get_angle() * DEGTORAD) * 0.2);
      this->set_dy(this->get_dy() + sin(this->get_angle() * DEGTORAD) * 0.2);
    }
    else
    {
      this->set_dx(this->get_dx() * 0.99);
      this->set_dy(this->get_dy() * 0.99);
    }

    int maxSpeed = 15;
    float speed = sqrt(this->get_dx() * this->get_dx() + this->get_dy() * this->get_dy());
    if (speed > maxSpeed)
    {
      this->set_dx(this->get_dx() * maxSpeed / speed);
      this->set_dx(this->get_dx() * maxSpeed / speed);
      
    }

    this->set_x(this->get_x() + this->get_dx());
    this->set_y(this->get_y() + this->get_dy());


    if (this->get_x() > W) this->set_x(0);
    if (this->get_x() < 0) this->set_x(W);
    if (this->get_y() > H) this->set_y(0);
    if (this->get_y() < 0) this->set_y(H);
  }
};

bool isCollide(Entity * a, Entity * b)
{
  return ((b->get_x() - a->get_x()) * (b->get_x() - a->get_x()) +
      (b->get_y() - a->get_y()) * (b->get_y() - a->get_y())) <
      ((a->get_R() + b->get_R()) * (a->get_R() + b->get_R()));
}



class Interface
{
public:
  float x, y;
  std::string name;
  
  Sprite sprite;
  Color color;
  sf::Text text;
public:
  Interface() : x(W/10), y(H/10), color(Color(175, 180, 240))
  {
    name = "interface";
  }
  Interface(float x_,float y_, Color color_): x(x_), y(y_), color(color_)
  {
    name= "interface";
  }


  void set_position(int x_,int y_)
  {
    x = x_;
    y = y_;
  }

  auto get_x()
  {
    return x;
  }
  auto get_y()
  {
    return y;
  }

};


class Interface_score : public Interface
{
public:
  Interface_score()
  {
    Interface();
    name = "interface_score";
  }
  Interface_score(float x_, float y_, Color color_)
  {
    x = x_;
    y = y_;
    color = color_;
    name= "interface_score";
  }

  void draw(RenderWindow &app, int score = 0)
  {
    sprite.setPosition(this->x - 110, this->y + 25);
    Texture t;
    t.loadFromFile("/Resources/images/scoreframe.png");
    sprite.setTexture(t);
    sprite.scale(0.2, 0.13);
    app.draw(sprite);

    sf::Font font;
    if(!font.loadFromFile("/Resources/images/sansation.ttf"))
    {
      std::cout << "oops!";
    }
    
    sf::Text text(std::to_string(score), font, 60);
    text.setPosition(this->x - 50, this->y + 40);
    text.sf::Text::setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    app.draw(text);
  }
  
};

class Interface_health : public Interface
{
  
public:
  Interface_health()
  {
    Interface();
    name = "interface_health";
  }
  Interface_health(float x_, float y_, Color color_)
  {
    x = x_;
    y = y_;
    color = color_;
    name = "interface_health";
    
  }

  void draw(RenderWindow &app, int live = 0)
  {
      const int w = 500;
    sprite.setPosition(this->x, this->y);
      sprite.setScale(0.5, 0.5);
    Texture t;
    t.loadFromFile("/Resources/images/healthbar.png");
    t.setSmooth(true);
    
    if (live == 3)
    {
        sprite.setTextureRect(IntRect(x, y, w, w));
        sprite.setTexture(t);
        app.draw(sprite);
    }
    if (live == 2)
    {
        sprite.setTextureRect(IntRect(x, y, w * 0.66, w));
        sprite.setTexture(t);
        app.draw(sprite);
    }
    if (live == 1)
    {
        sprite.setTextureRect(IntRect(x, y, w * 0.33, w));
        sprite.setTexture(t);
        app.draw(sprite);
    }
    

    sf::Font font;
    if(!font.loadFromFile("/Resources/images/sansation.ttf"))
    {
      std::cout << "oops!";
    }
    
    app.draw(text);
  }
  
};




int main()
{
  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::uniform_int_distribution<> un_distrib_W(0, W);
  std::uniform_int_distribution<> un_distrib_H(0, H);
  std::uniform_real_distribution<> un_distrib_360(0, 360);

  RenderWindow app(VideoMode(W, H), "ASTEROIDS");
  app.setFramerateLimit(120);

  Texture t1, t2, t3, t4, t5, t6, t7, t8;
  t1.loadFromFile("/Resources/images/spaceship.png");
  t2.loadFromFile("/Resources/images/background.jpg");
  t3.loadFromFile("/Resources/images/explosions/type_C.png");
  t4.loadFromFile("/Resources/images/rock.png");
  t5.loadFromFile("/Resources/images/fire_blue.png");
  t6.loadFromFile("/Resources/images/rock_small.png");
  t7.loadFromFile("/Resources/images/explosions/type_B.png");
  t8.loadFromFile("/Resources/images/Cookie.png");

  t1.setSmooth(true);
  t2.setSmooth(true);

  Sprite background(t2);

  Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
  Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
  Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
  Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
  Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
  Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
  Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
  Animation sHealer(t8, 0, 0, 150, 150, 1, 0.2, 0.25, 0.25);

  SoundBuffer shootBuffer;
  shootBuffer.loadFromFile("/Resources/images/shot.wav");
  Sound shoot(shootBuffer);
    shoot.setVolume(40);
  
  SoundBuffer healthBuffer;
  healthBuffer.loadFromFile("/Resources/images/Rise03.wav");
  Sound health(healthBuffer);

  SoundBuffer explosionBuffer;
  explosionBuffer.loadFromFile("/Resources/images/Explosion.wav");
  Sound explosion(explosionBuffer);
    explosion.setVolume(30);
  
  SoundBuffer endBuffer;
  endBuffer.loadFromFile("/Resources/images/gamover.wav");
  Sound end(endBuffer);
    end.setVolume(600);

  
  Music music;
  music.openFromFile("/Resources/images/DOOM.ogg");
  music.play();
  
    
    std::vector<Entity*> entities;

  for (int i = 0; i < 2; i++)
  {
    Entity * a = new asteroid;

    a->settings(sRock, un_distrib_W(mersenne), un_distrib_H(mersenne), un_distrib_360(mersenne), 25);
    entities.push_back(a);
  }

    player * p = new player;
  p->settings(sPlayer, 200, 200, 0.0, 20);
  entities.push_back(p);


  while (app.isOpen() && (!close))
  {
    Event event;
    while (app.pollEvent(event))
    {
      if (event.type == Event::Closed)
        app.close();

      if (event.type == Event::KeyPressed)
        if (event.key.code == Keyboard::L)
        {
            Entity * b = new bullet;
          b->settings(sBullet, p->get_x(), p->get_y(), p->get_angle(), 10);
          entities.push_back(b);

          shoot.play();
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::D))
      p->set_angle(p->get_angle() + 3.0);
    if (Keyboard::isKeyPressed(Keyboard::A))
      p->set_angle(p->get_angle() - 3.0);
    if (Keyboard::isKeyPressed(Keyboard::W))
      p->set_thrust(true);
    else
      p->set_thrust(false);
      
    for (auto a : entities)
      for (auto b : entities)
      {
        if (a->get_name() == "asteroid" && b->get_name() == "bullet")//
          if (isCollide(a,b))
          {
            a->set_life(0);
            b->set_life(0);

              Entity * e = new Entity;
            e->settings(sExplosion, a->get_x(), a->get_y());
            e->set_name("explosion");
            entities.push_back(e);

            for (int i = 0; i < 2; i++)
            {

              if (a->get_R() == 15)
              {
                p->add_scores(10);
                continue;

              }
              p->add_scores(5);
              
              
                Entity * e2 = new asteroid;
              e2->settings(sRock_small, a->get_x(), a->get_y(), un_distrib_360(mersenne), 15);
              entities.push_back(e2);
            }
            explosion.play();
          }

        if (a->get_name() == "player" && b->get_name() == "asteroid")
          if (isCollide(a, b))
          {
            b->set_life(0);

            Entity * e = new Entity;
            e->settings(sExplosion_ship, a->get_x(), a->get_y());
            e->set_name("explosion");
            entities.push_back(e);

            if(p->get_life() > 1)
            {
              p->sub_life(1);
              p->settings(sPlayer, W / 2, H / 2, 0, 20);
              p->set_dx(0);
              p->set_dy(0);
            }
            else
            {
              close = true;
            }
            
            explosion.play();
          }

        if (a->get_name() == "player" && b->get_name() == "healer")
          if (isCollide(a, b))
          {
            b->set_life(0);

            if(p->get_life() < 3)
            {
              p->add_life(1);
            }
            
            health.play();
            
          }
      }


    if (p->get_thrust())  p->anim = sPlayer_go;
    else   p->anim = sPlayer;


    for (auto e : entities)
      if (e->get_name() == "explosion")
        if ((e->get_anim()).isEnd())
          e->set_life(0);


    if (un_distrib_W(mersenne) % 100 == 0)
    {
        Entity * a = new asteroid;
      a->settings(sRock, 0, un_distrib_H(mersenne), un_distrib_360(mersenne), 25);
      entities.push_back(a);
    }
    if (un_distrib_W(mersenne) % 150 == 0)
    {
        Entity * h = new healer;
      h->settings(sHealer, 0, un_distrib_H(mersenne), un_distrib_360(mersenne), 25);
      entities.push_back(h);
    }

    for (auto i = entities.begin(); i != entities.end();)
    {
      Entity *e = * i;

      e->update();
      e->anim.update();

      if (e->get_life() == 0)
      {
        i = entities.erase(i);
      }
      else
        i++;
    }


    app.draw(background);
    for (auto i : entities)
      i->draw(app);
    p->draw(app);

    Interface_score insc(W-100,0, Color(175, 180, 240));
    insc.draw(app, p->get_scores());

    Interface_health inhe(0,0, Color(175, 180, 240));
    inhe.draw(app, p->get_life());
   
    app.display();
  }

  music.pause();
  end.play();
    
    std::cout << "SCORE: " << p->get_scores() << std:: endl;
  
  std::chrono::seconds pause(2);
  std::this_thread::sleep_for(pause);

  return 0;
}
