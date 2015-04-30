#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetHP(300);
  player->SetPosition(player_pos);
  
  cout<<"Player HP "<<player->HP()<<endl;

  const int number_of_aliens = 10;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(rand()%600+32, rand()%400+600);
    alien->SetHP(60);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2(300.0f, 450.0f);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
    break;
  case SFEVENT_PLAYER_UP:
    player->GoNorth();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  int w, h;
  SDL_GetRendererOutputSize(sf_window -> getRenderer(), &w, &h);
  for(auto p: projectiles) {
    p->GoNorth();
    auto p_pos=p->GetPosition();
    if(p_pos.getY()>h){p->HandleCollision();}
  }

  for(auto c: coins) {
    c->GoSouth();
    if (player->CollidesWith(c)){
	c->HandleCollision();
	cout<<"You've aquired a coin!"<<endl;}
  }

  // Update enemy positions
  for(auto a : aliens) {
    a->GoSouth();
    if(player-> CollidesWith(a)){
	   player->SetHP(player->HP()-30);
	  cout<<player->HP()<<" HP Remaining!"<<endl;
	  
	  	if(player->HP()<=0){
	cout<<"Game Over!"<<endl;
	is_running=false;
}
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
		cout<<"An Enemy has been slain!"<<endl;
	
        p-> HandleCollision();
        a-> HandleCollision();
      }
    }
  }

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
}


  list<shared_ptr<SFAsset>> p_remove;
  for(auto p : projectiles) {
    if(p->IsAlive()) {
      p_remove.push_back(p);
    }
  }
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(p_remove);
}




void SFApp::OnRender(){
  SDL_RenderClear(sf_window->getRenderer());
  int w, h;

  // draw the player
  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender();
  }

  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}


