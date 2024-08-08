#include "bn_core.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_regular_bg_ptr.h"
#include "bn_math.h"
#include "bn_regular_bg_item.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_items_tewts256.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_regular_bg_items_bg.h"
#include "common_variable_8x8_sprite_font.h"
#include "common_fixed_8x16_sprite_font.h"
#include "common_info.h"
#include "bn_log.h"

#include "bn_sprite_items_paddle.h"
#include "bn_sprite_items_ball.h"
#include "bn_sprite_items_larryball.h"

int main()
{
    bn::core::init();
    // Create a background at position 0,0.
    // We can move it around, but we don't want to for now.
    // ...
    // Notice that we're creating a "ptr" (pointer) object, which is
    // populated with the output of an "item" object's create function.
    // 'sprite_ptr' != 'sprite_item'
    bn::regular_bg_ptr bg = bn::regular_bg_items::tewts256.create_bg(0, 0);

    bn::sprite_ptr left_paddle = bn::sprite_items::paddle.create_sprite(-140, 0);
    bn::sprite_ptr right_paddle = bn::sprite_items::paddle.create_sprite(140, 0);

    right_paddle.set_horizontal_flip(true);

    bool useLarryBall = false;
    bn::sprite_ptr ball = bn::sprite_items::ball.create_sprite(0, 0);

    bn::random random;

    int score = 0;
    bool enemy_moving_up = true;

    double delta_x = 0;
    double delta_y = 0;
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // common::info info(info_text_lines, text_generator);
    bn::vector<bn::sprite_ptr, 16> _score_text_sprites;
    text_generator.generate(-6* 16, -68, "(Press A to start)", _score_text_sprites);
    while(true)
    {   
        if(bn::keypad::start_pressed())
        {
            
            bn::fixed current_x = ball.x();
            bn::fixed current_y = ball.y();
            if (useLarryBall) {
                useLarryBall = false;
                ball = bn::sprite_items::ball.create_sprite(0, 0);
            }
            else {
                useLarryBall = true;
                ball = bn::sprite_items::larryball.create_sprite(0, 0);
            }
            ball.set_position(current_x, current_y);
        }
        if(bn::keypad::up_held() && left_paddle.y() > -67)
        {
            left_paddle.set_y(left_paddle.y() - 1);
        }
        else if(bn::keypad::down_held() && left_paddle.y() < 67)
        {
            left_paddle.set_y(left_paddle.y() + 1);
        }

        if(enemy_moving_up) 
        {
            if(right_paddle.y() > -67)
            {
                right_paddle.set_y(right_paddle.y() - 1);
            }
            else 
            {
                enemy_moving_up = false;
            }
        }
        else {
            if(right_paddle.y() < 67)
            {
                right_paddle.set_y(right_paddle.y() + 1);
            }
            else {
                enemy_moving_up = true;
            }
        }

        if(bn::keypad::a_pressed() && delta_x == 0 && delta_y == 0)
        {
            _score_text_sprites.clear();
            bn::string<32> score_text = "Score: " + bn::to_string<32>(score);
            text_generator.generate(-6*16, -68, score_text, _score_text_sprites);
            delta_x = 1;
            delta_y = 1;

            while(delta_x == 0 || delta_y == 0) {
                delta_x = (random.get_int() % 5) - 2;
                delta_y = (random.get_int() % 5) - 2;
            }
            bn::sound_items::ping.play();
        }
        
        ball.set_x(ball.x() + delta_x);
        ball.set_y(ball.y() + delta_y);

        if(ball.y() > 67)
        {
            delta_y += .25;
            delta_y = delta_y * -1;
            bn::sound_items::ping.play();
        }
        else if(ball.y() < -67)
        {
            delta_y -= .25;
            delta_y = delta_y * -1;
            bn::sound_items::ping.play();
        }

        if(ball.x() < -110) 
        {
            if(bn::abs(ball.y() - left_paddle.y()) < 32)
            {
                delta_x -= .25;
                delta_x = delta_x * -1;
                bn::sound_items::pong.play();
            } 
            else {
                score--;
                ball.set_position(0, 0);
                delta_x = 0;
                delta_y = 0;
                _score_text_sprites.clear();
                bn::string<32> txt_score = "Score: " + bn::to_string<32>(score) + " (Press A)";
                text_generator.generate(-4* 16, -68, txt_score, _score_text_sprites);
            }
        }
        else if(ball.x() > 110) 
        {
            if(bn::abs(ball.y() - right_paddle.y()) < 32)
            {
                delta_x += .25;
                delta_x = delta_x * -1;
                bn::sound_items::pong.play();
            } 
            else {
                score++;
                ball.set_position(0, 0);
                delta_x = 0;
                delta_y = 0;
                _score_text_sprites.clear();
                bn::string<32> txt_score = "Score: " + bn::to_string<32>(score) + " (Press A)";
                text_generator.generate(-4* 16, -68, txt_score, _score_text_sprites);
            }
        }

        if(bn::keypad::b_pressed()) {
            BN_LOG("Score: ", score);
        }
        bn::core::update();
    }
}
