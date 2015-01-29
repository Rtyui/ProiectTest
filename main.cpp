#include<allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_image.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_font.h>
#include<stdio.h>
#include<iostream>
#include<fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include<stdlib.h>
#define screenWidth 800
#define screenHeight 640
#define blockWidth 50
#define blockHeight 50
#define mapWidth 100
#define mapHeight 100
#define bulletMaxDistance screenWidth

typedef struct {
	float x;
	float y;
	short type;

} blocks;
typedef struct {
	float bulletx;
	float bullety;
	float bulletInitialx;
	float bulletInitialy;
	short bulletType;
} bullet;
typedef struct {
	float x;
	float y;
	float hp;
	float atack;
	short type;
	ALLEGRO_BITMAP *show;
	float sourcex;
	float sourcey;
	short dir;
	bullet b;
} acolytes;



void cameraUpdate(float *cameraPosition, float x,float y, float width , float height)
{
	cameraPosition[0] = -(screenWidth/2) + (x + width/2);
	cameraPosition[1] = -(screenHeight/2) + (y + height/2);
	if(cameraPosition[0] < 0)
		cameraPosition[0] = 0;
	if(cameraPosition[1] < 0)
		cameraPosition[1] = 0;
	if(cameraPosition[0] > mapWidth*blockWidth-screenWidth)
		cameraPosition[0] = mapWidth*blockWidth-screenWidth;
	if(cameraPosition[1] > mapHeight*blockHeight-screenHeight)
		cameraPosition[1] = mapHeight*blockHeight-screenHeight;

}
void loadMap(const char *filename , blocks block[mapHeight][mapWidth])
{
	std::ifstream openfile(filename);
	for(int i=0;i<mapHeight;i++)
		for(int j=0;j<mapWidth;j++)
			openfile >> block[i][j].type;
}





enum KEYS { DOWN , LEFT , RIGHT , UP };


int main(int argc,char* argv[]){
	FreeConsole();
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_EVENT eve;
	ALLEGRO_TIMER *timer;
	ALLEGRO_TIMER *frameTimer;
	ALLEGRO_TRANSFORM camera;

	float sourcex=0, sourcey=0, width, height, x=screenWidth/2, y=screenHeight/2, acWidth, acHeight;

	float moveSpeed=5, dirUpdate=0, bulletx=0, bullety=0, bulletInitialx=0, bulletInitialy=0;

	float hp=100, atack, maxhp=100;

	bool redraw=false, done=false, down=false, up=false, left=false, right=false, bulletExist=false;

	short dir=4,playerType=3,bulletType=0,bulletDir=0,dirAux;
	short red[10],green[10],blue[10];
	red[3]=255,green[3]=127,blue[3]=36;
	red[4]=35,green[4]=35,blue[4]=142;
	
	float cameraPosition[2] = { 0, 0 };

	blocks block[mapHeight][mapWidth];
	acolytes ac[11];

	for(int i=0;i<mapHeight;i++)
		for(int j=0;j<mapWidth;j++)
		{
			block[i][j].x=j*blockWidth;
			block[i][j].y=i*blockHeight;
		}

	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_mouse();
	al_install_audio();
	al_init_acodec_addon();
	event_queue = al_create_event_queue();
	timer = al_create_timer(1 / 60.0);
	frameTimer = al_create_timer(1 / 10.0);
	display=al_create_display(screenWidth,screenHeight);
	al_set_display_flag(display, ALLEGRO_FULLSCREEN, true);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_register_event_source(event_queue , al_get_keyboard_event_source());
	al_register_event_source(event_queue , al_get_mouse_event_source());
	al_register_event_source(event_queue , al_get_timer_event_source(timer));
	al_register_event_source(event_queue , al_get_timer_event_source(frameTimer));
	ALLEGRO_BITMAP *player = al_load_bitmap("player.png");
	ALLEGRO_BITMAP *tree = al_load_bitmap("tree.png");
	ALLEGRO_BITMAP *grass = al_load_bitmap("grass.png");
	ALLEGRO_BITMAP *house = al_load_bitmap("casa1.png");
	ALLEGRO_BITMAP *fire = al_load_bitmap("fire_orb.png");
	ALLEGRO_BITMAP *water = al_load_bitmap("water_orb.png");
	ALLEGRO_BITMAP *nature = al_load_bitmap("nature_orb.png");
	ALLEGRO_BITMAP *wind = al_load_bitmap("wind_orb.png");
	ALLEGRO_BITMAP *stone = al_load_bitmap("stone_orb.png");
	ALLEGRO_BITMAP *magic = al_load_bitmap("magic_orb.png");
	ALLEGRO_BITMAP *light = al_load_bitmap("light_orb.png");
	ALLEGRO_BITMAP *dark = al_load_bitmap("dark_orb.png");
	ALLEGRO_BITMAP *river = al_load_bitmap("water.png");
	ALLEGRO_BITMAP *podv = al_load_bitmap("pod1.png");
	ALLEGRO_BITMAP *podo = al_load_bitmap("pod2.png");
	ALLEGRO_BITMAP *tree2 = al_load_bitmap("tree2.png");
	ALLEGRO_BITMAP *tree3 = al_load_bitmap("tree3.png");
	ALLEGRO_BITMAP *house2 = al_load_bitmap("casa2.png");
	ALLEGRO_BITMAP *house3 = al_load_bitmap("casa3.png");
	ac[3].show = al_load_bitmap("fire_acolyte.png");
	ac[4].show = al_load_bitmap("water_acolyte.png");
	ac[5].show = al_load_bitmap("nature_acolyte.png");
	ac[6].show = al_load_bitmap("wind_acolyte.png");
	ac[7].show = al_load_bitmap("stone_acolyte.png");
	ac[9].show = al_load_bitmap("light_acolyte.png");
	ac[10].show = al_load_bitmap("dark_acolyte.png");
	acWidth = al_get_bitmap_width(ac[3].show)/4;
	acHeight = al_get_bitmap_height(ac[3].show)/4;



	for(int i=3;i<11;i++)
	{
		if(i!=8)
		{
			ac[i].sourcey=0;
			ac[i].sourcex=0;
			ac[i].dir=rand()%4;
			ac[i].sourcey=ac[i].dir*acHeight;
			ac[i].type = i;
			ac[i].x=rand()%(blockWidth*mapWidth-25*blockWidth);
			ac[i].y=rand()%(blockHeight*mapHeight-15*blockHeight);
			ac[i].x=x;
			ac[i].y=y;
		}

	}



	

	//al_hide_mouse_cursor(display);	
	al_clear_to_color(al_map_rgb(0,0,0));
	al_start_timer(timer);
	al_start_timer(frameTimer);
	width=al_get_bitmap_width(player)/3,height=al_get_bitmap_height(player)/4;
	dirUpdate=al_get_bitmap_width(player)/3;
	loadMap("map.txt",block);
	while(!done)
	{
		al_wait_for_event(event_queue , &eve);
		
		

		if(eve.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;
			if(eve.timer.source == timer)
			{
				if(down) y+=moveSpeed;
				else if(up) y-=moveSpeed;
				else if(left) x-=moveSpeed;
				else if(right) x+=moveSpeed;

				for(int i=3;i<11;i++)
				{
					if(i!=8){
					if(ac[i].dir == DOWN) ac[i].y+=moveSpeed-3;
					else if(ac[i].dir == LEFT) ac[i].x-=moveSpeed-3;
					else if(ac[i].dir == RIGHT) ac[i].x+=moveSpeed-3;
					else if(ac[i].dir == UP) ac[i].y-=moveSpeed-3;
					if(((block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].type!=0
						&& ac[i].y<=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].y+blockHeight
						&& ac[i].y>=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].y
						&& ac[i].x>=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].x
						&& ac[i].x<=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].x+blockWidth)
						|| (block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].type!=0
						&& ac[i].y<=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].y+blockHeight
						&& ac[i].y>=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].y
						&& ac[i].x+acWidth>=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].x
						&& ac[i].x+acWidth<=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].x+blockWidth))
						&& ac[i].dir == UP)
					{
						ac[i].y=block[(int)((ac[i].y-blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].y+blockHeight+1;
						ac[i].dir+=1;
						if(ac[i].dir>=3) ac[i].dir=0;
					}

					if(((block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].type!=0
						&& ac[i].y<=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].y+blockHeight
						&& ac[i].y>=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].y
						&& ac[i].x>=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].x
						&& ac[i].x<=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].x+blockWidth)
						|| (block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].type!=0
						&& ac[i].y+acHeight<=block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].y+blockHeight
						&& ac[i].y+acHeight>=block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].y
						&& ac[i].x>=block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].x
						&& ac[i].x<=block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].x+blockWidth))
						&& ac[i].dir == LEFT)
					{
						ac[i].x=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x-blockWidth/2)/blockWidth)].x+blockWidth+1;
						ac[i].dir+=1;
						if(ac[i].dir>=3) ac[i].dir=0;
					}

					if(((block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].type!=0
						&& ac[i].y+acHeight>=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].y
						&& ac[i].y+acHeight<=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].y+blockHeight
						&& ac[i].x>=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].x
						&& ac[i].x<=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].x+blockWidth)
						|| (block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].type!=0
						&& ac[i].y+acHeight<=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].y+blockHeight
						&& ac[i].y+acHeight>=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].y
						&& ac[i].x+acWidth>=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].x
						&& ac[i].x+acWidth<=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+acWidth)/blockWidth)].x+blockWidth))
						&& ac[i].dir == DOWN)
					{
						ac[i].y=block[(int)((ac[i].y+acHeight+blockHeight/2)/blockHeight)][(int)((ac[i].x+1)/blockWidth)].y-acHeight-1;
						ac[i].dir+=1;
						if(ac[i].dir>=3) ac[i].dir=0;
					}

					if(((block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].type!=0
						&& ac[i].y>=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].y
						&& ac[i].y<=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].y+blockHeight
						&& ac[i].x+acWidth>=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].x
						&& ac[i].x+acWidth<=block[(int)((ac[i].y+1)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].x+blockWidth)
						|| (block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].type!=0
						&& ac[i].y+acHeight<=block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].y+blockHeight
						&& ac[i].y+acHeight>=block[(int)((ac[i].y+acHeight)/blockHeight)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].y
						&& ac[i].x+acWidth>=block[(int)((ac[i].y+acHeight)/blockHeight+blockHeight/2)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].x
						&& ac[i].x+acWidth<=block[(int)((ac[i].y+acHeight)/blockHeight+blockHeight/2)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].x+blockWidth))
						&& ac[i].dir == RIGHT)
					{
						ac[i].x=block[(int)((ac[i].y+acHeight)/blockHeight+blockHeight/2)][(int)((ac[i].x+acWidth+blockWidth/2)/blockWidth)].x-acWidth-1;
						ac[i].dir+=1;
						if(ac[i].dir>=3) ac[i].dir=0;
					}


					if(ac[i].x<=0) { ac[i].x = 1; ac[i].dir+=1;if(ac[i].dir==3) ac[i].dir=0;}
					if(ac[i].x+acWidth>=mapWidth*blockWidth) 
					{ ac[i].x = mapWidth*blockWidth-acWidth-1;ac[i].dir+=1;if(ac[i].dir==3) ac[i].dir=0;}
					if(ac[i].y<=0) { ac[i].y = 1; ac[i].dir+=1;if(ac[i].dir==3) ac[i].dir=0; }
					if(ac[i].y+acHeight>=mapHeight*blockHeight)
					{ ac[i].y = mapHeight*blockHeight-acHeight-1; ac[i].dir+=1;if(ac[i].dir==3) ac[i].dir=0;}

					if(rand()%2+rand()%2+rand()%2+rand()%2+rand()%2+rand()%2+rand()%2==7) ac[i].dir = rand()%4;
					ac[i].sourcey=ac[i].dir*acHeight;
					}
				}

				

				if( x < 0 ) x = 0;
				if( y < 0 ) y = 0;
				if( x > mapWidth*blockWidth-width) x = mapWidth*blockWidth-width;
				if( y > mapHeight*blockHeight-height) y = mapHeight*blockHeight-height;

				if(bulletExist)
				{
					if(bulletDir == 0) bullety+=10;
					else if(bulletDir == 1) bulletx-=10;
					else if(bulletDir == 2) bulletx+=10;
					else if(bulletDir == 3) bullety-=10;
					if(abs(bulletx - bulletInitialx)>=bulletMaxDistance 
						|| abs(bullety - bulletInitialy)>=bulletMaxDistance) bulletExist=false;
					if(block[(int)(bullety/blockHeight)][(int)(bulletx/blockWidth)].type!=0 
						&& block[(int)(bullety/blockHeight)][(int)(bulletx/blockWidth)].type!=11)
						bulletExist=false;

				}


				if((((block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==1 
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==11 
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==2
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==20
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==14
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==15
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==16
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==17)
					&& x>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x 
					&& x<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x+blockWidth 
					&& y<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y+blockHeight 
					&& y>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y) 
					|| ((block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==1
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==11
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==2
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==20
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==14
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==15
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==16
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==17)
					&& x+width>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x 
					&& x+width<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x+blockWidth 
					&& y<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y+blockHeight 
					&& y>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y)) && dir == 3)
				{
					y=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+blockWidth/2)/blockWidth)].y + blockHeight+1;
				}
				if(((block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==3
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==4
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==5
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==6
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==7
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==8
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==9
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type==10)
					&& x>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x 
					&& x<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x+blockWidth 
					&& y<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y+blockHeight 
					&& y>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y) && dir==3)
					playerType=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type;
				if( ((block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==3
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==4
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==5
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==6
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==7
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==8
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==9
					|| block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==10)
					&& x+width>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x 
					&& x+width<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x+blockWidth 
					&& y<=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y+blockHeight 
					&& y>=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y) && dir == 3)
					playerType=block[(int)((y-blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type;
					
				
				if((((block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==1
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==11
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==2
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==20
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==14
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==15
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==16
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==17)
					&& x+width>=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x 
					&& x+width<=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x+blockWidth 
					&& y>=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y 
					&& y<=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y+blockHeight) 
					|| ((block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==1
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==11
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==2
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==20
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==14
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==15
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==16
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==17)
					&& y+height>=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y
					&& y+height<=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y+blockHeight
					&& x+width>=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x
					&& x+width<=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x+blockWidth)) 
					&& dir == 2)
				{
					x=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x-width-1;
				}
				if((((block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==3
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==4
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==5
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==6
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==7
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==8
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==9
					|| block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==10)
					&& x+width>=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x 
					&& x+width<=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x+blockWidth 
					&& y>=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y 
					&& y<=block[(int)((y+1)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y+blockHeight) 
					|| ((block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==3
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==4
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==5
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==6
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==7
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==8
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==9
					|| block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].type==10)
					&& y+height>=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y
					&& y+height<=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].y+blockHeight
					&& x+width>=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x
					&& x+width<=block[(int)((y+height)/blockHeight)][(int)((x+width+blockWidth/2)/blockWidth)].x+blockWidth)) 
					&& dir == 2)
				{
					
				}
				if( (((block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==1
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==11
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==2
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==20
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==14
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==15
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==16
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==17)
					&& x>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x
					&& x<=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x+blockWidth
					&& y+height>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y
					&& y+height <= block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y+blockHeight)
					|| ((block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==1
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==11
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==2
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==20
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==14
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==15
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==16
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==17)
					&& x+width>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x
					&& x+width<=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x+blockWidth
					&& y+height>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y
					&& y+height <= block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y+blockHeight)) 
					&& dir==0)
				{
					y=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y-height-1;
				}
				if( (((block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==3
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==4
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==5
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==6
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==7
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==8
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==9
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].type ==10)
					&& x>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x
					&& x<=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].x+blockWidth
					&& y+height>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y
					&& y+height <= block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+1)/blockWidth)].y+blockHeight)
					|| ((block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==3
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==4
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==5
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==6
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==7
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==8
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==9
					|| block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].type==10)
					&& x+width>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x
					&& x+width<=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].x+blockWidth
					&& y+height>=block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y
					&& y+height <= block[(int)((y+height+blockHeight/2)/blockHeight)][(int)((x+width)/blockWidth)].y+blockHeight)) 
					&& dir==0)
				{
					
				}
				if((((block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==1
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==11
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==2
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==20
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==14
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==15
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==16
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==17)
					&& x<=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x+blockWidth
					&& x>=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x
					&& y>=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y
					&& y<=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y+blockHeight)
					|| ((block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==1
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==11
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==2
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==20
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==14
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==15
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==16
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==17)
					&& x<=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x+blockWidth
					&& x>=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x
					&& y+height>=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y
					&& y+height<=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y+blockHeight)) && dir==1)
				{
					x=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x+blockWidth;
				}
				if((((block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==3
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==4
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==5
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==6
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==7
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==8
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==9
					|| block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==10)
					&& x<=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x+blockWidth
					&& x>=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x
					&& y>=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y
					&& y<=block[(int)((y+1)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y+blockHeight)
					|| ((block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==3
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==4
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==5
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==6
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==7
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==8
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==9
					|| block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].type ==10)
					&& x<=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x+blockWidth
					&& x>=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].x
					&& y+height>=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y
					&& y+height<=block[(int)((y+height)/blockHeight)][(int)((x-blockWidth/2)/blockWidth)].y+blockHeight)) && dir==1)
				{
					
				}
				cameraUpdate(cameraPosition, x, y, width, height);

				al_identity_transform(&camera);

				al_translate_transform(&camera , -cameraPosition[0] , -cameraPosition[1]);

				al_use_transform(&camera);

				sourcey=dir*al_get_bitmap_height(player)/4;

			}


			else if(eve.timer.source == frameTimer)
			{
				
					sourcex+=width;//al_get_bitmap_width(player)/3;
					if(sourcex >= al_get_bitmap_width(player))
						sourcex=0;
					for(int i=3;i<11;i++)
					{
						if(i!=8)
						{
							ac[i].sourcex+=acWidth;
							if(ac[i].sourcex >= acWidth*4)
								ac[i].sourcex=0;
						}
					}
				
			}
			
			
			

		}
		else if(eve.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			
				
				if(eve.keyboard.keycode == ALLEGRO_KEY_UP)
				{
					up=true;
					right=false;
					down=false;
					left=false;
					dir=UP;
					dirAux=dir;
				}
				else if(eve.keyboard.keycode == ALLEGRO_KEY_RIGHT)
				{
					right=true;
					up=false;
					down=false;
					left=false;
					dir=RIGHT;
					dirAux=dir;
				}
				else if(eve.keyboard.keycode == ALLEGRO_KEY_DOWN)
				{
					down=true;
					up=false;
					right=false;
					left=false;
					dir=DOWN;
					dirAux=dir;
				}
				else if(eve.keyboard.keycode == ALLEGRO_KEY_LEFT)
				{
					left=true;
					up=false;
					right=false;
					down=false;
					dir=LEFT;
					dirAux=dir;
				}
				else if(eve.keyboard.keycode == ALLEGRO_KEY_SPACE)
				{
					if(!bulletExist)
					{
						bulletExist = true;
						bulletInitialx = x+width/2;
						bulletInitialy = y+height/2;
						bulletx = x+width/2;
						bullety = y+height/2;
						bulletDir=dirAux;
					}
				}
				else if(eve.keyboard.keycode == ALLEGRO_KEY_P)
				bulletExist=false;

				else if(eve.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					done=true;
				
				
		}
		else if(eve.type == ALLEGRO_EVENT_KEY_UP)
		{
			
			if(eve.keyboard.keycode == ALLEGRO_KEY_UP) up=false;
			else if(eve.keyboard.keycode == ALLEGRO_KEY_RIGHT) right=false;
			else if(eve.keyboard.keycode == ALLEGRO_KEY_DOWN) down=false;
			else if(eve.keyboard.keycode == ALLEGRO_KEY_LEFT) left=false;
			if(up) dir=UP;
			else if(down) dir=DOWN;
			else if(left) dir=LEFT;
			else if(right) dir=RIGHT;
			else dir=4;
					
		}
		
		

		if(redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;
			al_clear_to_color(al_map_rgb(0,0,0));
			for(int i=cameraPosition[1]/blockHeight;i<(cameraPosition[1]+screenHeight+blockHeight)/blockHeight;i++)
				for(int j=cameraPosition[0]/blockWidth;j<(cameraPosition[0]+screenWidth+blockWidth)/blockWidth;j++)
				{
			al_draw_scaled_bitmap(grass,0,0,al_get_bitmap_width(grass),al_get_bitmap_height(grass),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
				}

			for(int i=cameraPosition[1]/blockHeight;i<(cameraPosition[1]+screenHeight+blockHeight)/blockHeight;i++)
				for(int j=cameraPosition[0]/blockWidth;j<(cameraPosition[0]+screenWidth+blockWidth)/blockWidth;j++)
				{
					
					switch(block[i][j].type)
					{
					case(1): al_draw_scaled_bitmap(tree,0,0,al_get_bitmap_width(tree),al_get_bitmap_height(tree),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(2): al_draw_scaled_bitmap(house,0,0,al_get_bitmap_width(house),al_get_bitmap_height(house),block[i][j].x,block[i][j].y,2*blockWidth,2*blockHeight,NULL);
						break;
					case(14): al_draw_scaled_bitmap(tree2,0,0,al_get_bitmap_width(tree2),al_get_bitmap_height(tree2),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(15): al_draw_scaled_bitmap(tree3,0,0,al_get_bitmap_width(tree3),al_get_bitmap_height(tree3),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(16): al_draw_scaled_bitmap(house2,0,0,al_get_bitmap_width(house2),al_get_bitmap_height(house2),block[i][j].x,block[i][j].y,2*blockWidth,2*blockHeight,NULL);
						break;
					case(17): al_draw_scaled_bitmap(house3,0,0,al_get_bitmap_width(house3),al_get_bitmap_height(house3),block[i][j].x,block[i][j].y,2*blockWidth,2*blockHeight,NULL);
						break;
					case(3):al_draw_scaled_bitmap(fire,0,0,al_get_bitmap_width(fire),al_get_bitmap_height(fire),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(4):al_draw_scaled_bitmap(water,0,0,al_get_bitmap_width(water),al_get_bitmap_height(water),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(5):al_draw_scaled_bitmap(nature,0,0,al_get_bitmap_width(nature),al_get_bitmap_height(nature),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(6):al_draw_scaled_bitmap(wind,0,0,al_get_bitmap_width(wind),al_get_bitmap_height(wind),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(7):al_draw_scaled_bitmap(stone,0,0,al_get_bitmap_width(stone),al_get_bitmap_height(stone),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(8):al_draw_scaled_bitmap(magic,0,0,al_get_bitmap_width(magic),al_get_bitmap_height(magic),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(9):al_draw_scaled_bitmap(light,0,0,al_get_bitmap_width(light),al_get_bitmap_height(light),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(10):al_draw_scaled_bitmap(dark,0,0,al_get_bitmap_width(dark),al_get_bitmap_height(dark),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(11):al_draw_scaled_bitmap(river,0,0,al_get_bitmap_width(river),al_get_bitmap_height(river),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(12):al_draw_scaled_bitmap(podo,0,0,al_get_bitmap_width(podo),al_get_bitmap_height(podo),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					case(13):al_draw_scaled_bitmap(podv,0,0,al_get_bitmap_width(podv),al_get_bitmap_height(podv),block[i][j].x,block[i][j].y,blockWidth,blockHeight,NULL);
						break;
					}
				}
			for(int i=3;i<11;i++)
			{
				if(i!=8)
				{
					if(ac[i].x+acWidth>=cameraPosition[0]-1
					&& ac[i].x<=cameraPosition[0]+screenWidth+1
						&& ac[i].y+acHeight>=cameraPosition[1]-1
					&& ac[i].y<=cameraPosition[1]+screenHeight+1)
						al_draw_bitmap_region(ac[i].show,ac[i].sourcex,ac[i].sourcey,acWidth,acHeight,ac[i].x,ac[i].y,NULL);
				}
			}
			
			if(dir!=4)
			{
				al_draw_bitmap_region(player,sourcex,sourcey,width,height,x,y,NULL);
				
			}
			else al_draw_bitmap_region(player,al_get_bitmap_width(player)/3,0,width,height,x,y,NULL);
			al_draw_line(x,y-5,x+(hp*width)/maxhp,y-5,al_map_rgb(150,0,0),3);
			if(bulletExist) al_draw_filled_circle(bulletx,bullety,5,al_map_rgb(red[playerType],green[playerType],blue[playerType]));
			
			
			
			al_flip_display();

		}
			
		
}


		return 0;
		}