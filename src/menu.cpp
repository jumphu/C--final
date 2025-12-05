// menu_integrated.cpp
// �滻ԭ menu.cpp���������ϱ�������ť�����֡���Ⱦ
#include "Renderer.h"
#include "background_integrated.h"
#include "allbottums.h"
#include "music.h"

#include <conio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>

enum class AppState { MENU, DEMO_RUNNING, DEMO_PAUSED };
enum class ModelType { SLOPE = 0, COLLISION = 1 };

struct SlopeParams {
    double top_x, top_y, bottom_x, bottom_y, mu;
    BallData ball;
};

struct CollisionParams {
    BallData b1, b2;
};

int main() {
    const int WINW = 1024;
    const int WINH = 720;
    const double SCALE = 60.0;

    Renderer renderer(WINW, WINH, SCALE);

    // Integrated components
    DigitalRainBackgroundIntegrated bg;
    MusicPlayer musicBtn(920, 80, 36);

    AppState state = AppState::MENU;
    ModelType model = ModelType::SLOPE;

    // Default slope params
    SlopeParams slope;
    slope.top_x = 2.0; slope.top_y = 4.0;
    slope.bottom_x = 8.0; slope.bottom_y = 1.5;
    slope.mu = 0.15;
    slope.ball.radius = 0.18;
    slope.ball.mass = 0.5;
    slope.ball.color = RED;
    slope.ball.vx = slope.ball.vy = 0.0;
    slope.ball.x = slope.top_x;
    slope.ball.y = slope.top_y + slope.ball.radius + 0.001;

    // Default collision params
    CollisionParams coll;
    coll.b1.x = 2.5; coll.b1.y = 2.0; coll.b1.radius = 0.3; coll.b1.mass = 1.0; coll.b1.vx = 2.0; coll.b1.vy = 0; coll.b1.color = BLUE;
    coll.b2.x = 6.0; coll.b2.y = 2.0; coll.b2.radius = 0.4; coll.b2.mass = 1.5; coll.b2.vx = -1.0; coll.b2.vy = 0; coll.b2.color = GREEN;

    SlopeParams slope_init = slope;
    CollisionParams coll_init = coll;

    // init button modules positions
    initBtns(WINW, WINH);
    initBtns2(WINW, WINH);

    double simTime = 0.0;
    const double g = 9.81;
    const double dt = 0.016;
    bool quit = false;
    bool paused = false;

    int menuIndex = 0;
    int maxMenu = 3;

    // main loop
    while (!quit) {
        renderer.BeginFrame();

        // draw background first
        bg.UpdateAndDraw();

        // draw music button
        // We'll process mouse messages below; for now draw the button (music state drawn inside)
        musicBtn.Draw();

        if (state == AppState::MENU) {
            // Draw UI via renderer (title + placeholders)
            renderer.DrawText("Physics Visualizer - Demo Selection", 20, 12, 24);
            renderer.DrawButtonPlaceholder(30, 60, 300, 40, (model == ModelType::SLOPE ? "[X] Slope: ball rolls down" : "[ ] Slope: ball rolls down"));
            renderer.DrawButtonPlaceholder(350, 60, 300, 40, (model == ModelType::COLLISION ? "[X] Collision: two balls" : "[ ] Collision: two balls"));

            int y = 130;
            renderer.DrawText("Parameters (use LEFT/RIGHT change, UP/DOWN navigate)", 20, y, 14);
            y += 28;

            if (model == ModelType::SLOPE) {
                renderer.DrawText("Ramp top (m): x = " + std::to_string(slope.top_x) + "  y = " + std::to_string(slope.top_y), 30, y); y += 22;
                renderer.DrawText("Ramp bottom (m): x = " + std::to_string(slope.bottom_x) + "  y = " + std::to_string(slope.bottom_y), 30, y); y += 22;
                renderer.DrawText("Friction mu: " + std::to_string(slope.mu), 30, y); y += 22;
                renderer.DrawText("Ball radius (m): " + std::to_string(slope.ball.radius) + "  mass (kg): " + std::to_string(slope.ball.mass), 30, y); y += 22;

                // draw scene1 UI (4 buttons)
                drawBtns(); // uses initBtns positions
            }
            else {
                renderer.DrawText("Ball1: pos=(" + std::to_string(coll.b1.x) + "," + std::to_string(coll.b1.y) + ")  vx=" + std::to_string(coll.b1.vx), 30, y); y += 22;
                renderer.DrawText("Ball2: pos=(" + std::to_string(coll.b2.x) + "," + std::to_string(coll.b2.y) + ")  vx=" + std::to_string(coll.b2.vx), 30, y); y += 22;

                // draw scene2 UI (8 buttons)
                drawBtns2();
            }

            // draw preview area
            int previewX = 30;
            int previewY = 380;
            int previewW = 800;
            int previewH = 300;
            setlinecolor(BLACK);
            rectangle(previewX - 2, previewY - 2, previewX + previewW + 2, previewY + previewH + 2);

            if (model == ModelType::SLOPE) {
                RampData rp;
                rp.x1 = slope.top_x; rp.y1 = slope.top_y;
                rp.x2 = slope.bottom_x; rp.y2 = slope.bottom_y;
                rp.mu = slope.mu;
                renderer.DrawRamp(rp);
                BallData b = slope.ball;
                renderer.DrawBall(b);
            }
            else {
                renderer.DrawBall(coll.b1);
                renderer.DrawBall(coll.b2);
            }

            // keyboard handling (menu)
            if (_kbhit()) {
                int ch = _getch();
                if (ch == 0 || ch == 0xE0) {
                    int ch2 = _getch();
                    if (ch2 == 72) menuIndex = std::max(0, menuIndex - 1);
                    else if (ch2 == 80) menuIndex = std::min(maxMenu, menuIndex + 1);
                    else if (ch2 == 75) {
                        if (model == ModelType::SLOPE) {
                            if (menuIndex == 1) slope.top_x = std::max(0.2, slope.top_x - 0.1);
                            else if (menuIndex == 2) slope.bottom_x = std::max(0.2, slope.bottom_x - 0.1);
                            else if (menuIndex == 3) slope.mu = std::max(0.0, slope.mu - 0.01);
                        }
                        else {
                            if (menuIndex == 1) coll.b1.vx -= 0.1;
                            else if (menuIndex == 2) coll.b2.vx += 0.1;
                        }
                    }
                    else if (ch2 == 77) {
                        if (model == ModelType::SLOPE) {
                            if (menuIndex == 1) slope.top_x += 0.1;
                            else if (menuIndex == 2) slope.bottom_x += 0.1;
                            else if (menuIndex == 3) slope.mu += 0.01;
                        }
                        else {
                            if (menuIndex == 1) coll.b1.vx += 0.1;
                            else if (menuIndex == 2) coll.b2.vx -= 0.1;
                        }
                    }
                }
                else {
                    // normal key
                    if (ch == 13) { // Enter -> start
                        // reset sim state
                        simTime = 0.0;
                        // reset positions from parameters
                        slope.ball.x = slope.top_x;
                        slope.ball.y = slope.top_y + slope.ball.radius + 0.001;
                        slope.ball.vx = slope.ball.vy = 0.0;
                        coll.b1 = coll_init.b1;
                        coll.b2 = coll_init.b2;
                        state = AppState::DEMO_RUNNING;
                        paused = false;
                    }
                    else if (ch == 27) { // Esc
                        quit = true;
                    }
                    else if (ch == ' ') {
                        // toggle model selection quick
                        model = (model == ModelType::SLOPE ? ModelType::COLLISION : ModelType::SLOPE);
                    }
                    else if (ch == 'r' || ch == 'R') {
                        slope = slope_init;
                        coll = coll_init;
                    }
                    else if (ch == '1') {
                        model = ModelType::SLOPE;
                    }
                    else if (ch == '2') {
                        model = ModelType::COLLISION;
                    }
                }
            }

        }
        else if (state == AppState::DEMO_RUNNING || state == AppState::DEMO_PAUSED) {
            // Simulation drawing
            renderer.DrawText("Press Space to Pause/Resume. Press any key to restart from menu.", 20, 12, 14);

            if (model == ModelType::SLOPE) {
                RampData rp = buildRamp(slope);
                renderer.DrawRamp(rp);

                // Simulation of ball sliding down ramp (no rotational dynamics)
                // Parametrization: compute along-ramp coordinates
                double rx = rp.x2 - rp.x1;
                double ry = rp.y2 - rp.y1;
                double L = sqrt(rx * rx + ry * ry);
                double ux = rx / L, uy = ry / L; // unit vector from top->bottom

                // position along ramp s: we can store s via ball position projection
                double bx = slope.ball.x, by = slope.ball.y;
                // project (bx,by) onto ramp vector from top
                double vx = slope.ball.vx, vy = slope.ball.vy;

                // compute current s (meters) from top
                double sx = (bx - rp.x1) * ux + (by - rp.y1) * uy;

                if (!paused) {
                    // acceleration along ramp: a = g*( -sin(theta) ) + friction
                    // but careful with sign: if ramp vector points down, gravity component along ramp = -g * sin(alpha_ramp)
                    // compute angle of ramp relative to horizontal
                    double alpha = atan2(rp.y2 - rp.y1, rp.x2 - rp.x1); // radians
                    // component of gravity along ramp (toward bottom) = -g * sin(alpha_downward)
                    // Better: gravitational acceleration vector = (0, -g). project onto unit vector (ux,uy)
                    double ag = (0.0 * ux + (-g) * uy); // dot product gvec . u
                    // Normal force magnitude per unit mass = g * cos(theta_projection)
                    double normal_acc = fabs((0.0 * (-uy) + (-g) * ux)); // not used directly
                    // friction acceleration magnitude = mu * g * cos(theta) but direction opposite velocity along ramp
                    double cos_theta = fabs(((-g) * ux) / g); // not robust; simpler compute cos from geometry
                    // compute slope angle relative to horizontal
                    double slopeAngle = atan2(rp.y1 - rp.y2, rp.x2 - rp.x1); // angle downward
                    // Simpler and robust approach:
                    double angle_down = atan2(rp.y1 - rp.y2, rp.x2 - rp.x1); // positive if down to right
                    // Use scalar: gravity component along ramp toward bottom:
                    double grav_along = g * sin(atan2(rp.y1 - rp.y2, rp.x2 - rp.x1)); // might have sign issues
                    // We'll instead compute exactly: unit vector pointing down (from top->bottom) is u=(ux,uy)
                    // gravity vector = (0, -g)
                    double grav_comp = (0.0) * ux + (-g) * uy; // negative if points opposite to u; we want toward bottom => -grav_comp
                    // So acceleration along ramp toward bottom (positive if toward bottom):
                    double a_gravity_along = -grav_comp;
                    // friction acceleration magnitude:
                    double a_friction = slope.mu * g * fabs(ux * (-uy) + uy * ux); // this is hacky
                    // Better: friction = mu * g * cos(theta); cos(theta) = dot(n, gdir) ??? To avoid complexity, use:
                    // compute angle of ramp relative to horizontal
                    double theta = atan2(rp.y2 - rp.y1, rp.x2 - rp.x1); // top->bottom vector angle
                    double a_fric = slope.mu * g * cos(theta);
                    // Sign: friction opposes velocity along ramp
                    // current velocity along ramp:
                    double v_along = vx * ux + vy * uy;
                    double a_along = a_gravity_along - (v_along > 0 ? a_fric : -a_fric);

                    // update velocity and s using semi-implicit Euler
                    v_along += a_along * dt;
                    sx += v_along * dt;

                    // clamp
                    if (sx < 0) { sx = 0; v_along = 0; }
                    if (sx > L) { sx = L; v_along = 0; }

                    // compute new world position
                    double newx = rp.x1 + ux * sx;
                    double newy = rp.y1 + uy * sx;

                    slope.ball.x = newx;
                    slope.ball.y = newy;
                    slope.ball.vx = v_along * ux;
                    slope.ball.vy = v_along * uy;

                    simTime += dt;
                }

                // draw ball
                renderer.DrawBall(slope.ball);

                // check input for pause/resume/restart/return to menu
                if (_kbhit()) {
                    int c = _getch();
                    if (c == ' ') { paused = !paused; }
                    else if (c == 27) { // Esc return to menu
                        state = AppState::MENU;
                        // reset to initial
                        slope = slope_init;
                        paused = false;
                    }
                    else {
                        // any other key -> restart demo from top
                        slope = slope_init;
                        simTime = 0.0;
                    }
                }

            }
            else { // COLLISION model
                // Draw static ground and two balls
                // Integrate simple 1D elastic collision along x axis (y const)
                renderer.DrawBall(coll.b1);
                renderer.DrawBall(coll.b2);

                if (!paused) {
                    // update pos
                    coll.b1.x += coll.b1.vx * dt;
                    coll.b2.x += coll.b2.vx * dt;

                    // collision detection (circle overlap)
                    double dx = coll.b2.x - coll.b1.x;
                    double dy = coll.b2.y - coll.b1.y;
                    double dist = sqrt(dx * dx + dy * dy);
                    double minDist = coll.b1.radius + coll.b2.radius;
                    if (dist <= minDist) {
                        // simple elastic collision along the line connecting centers
                        // compute normal
                        double nx = dx / dist;
                        double ny = dy / dist;
                        // relative velocity along normal
                        double rvx = coll.b2.vx - coll.b1.vx;
                        double rvy = coll.b2.vy - coll.b1.vy;
                        double relVelAlong = rvx * nx + rvy * ny;
                        if (relVelAlong < 0) {
                            // compute impulse scalar (1D)
                            double e = 1.0; // restitution
                            double j = -(1 + e) * relVelAlong / (1.0 / coll.b1.mass + 1.0 / coll.b2.mass);
                            // apply impulse
                            coll.b1.vx -= j * nx / coll.b1.mass;
                            coll.b1.vy -= j * ny / coll.b1.mass;
                            coll.b2.vx += j * nx / coll.b2.mass;
                            coll.b2.vy += j * ny / coll.b2.mass;
                            // push them apart minimally
                            double overlap = minDist - dist;
                            coll.b1.x -= nx * overlap * (coll.b2.mass / (coll.b1.mass + coll.b2.mass));
                            coll.b2.x += nx * overlap * (coll.b1.mass / (coll.b1.mass + coll.b2.mass));
                        }
                    }
                }

                // key handling
                if (_kbhit()) {
                    int c = _getch();
                    if (c == ' ') paused = !paused;
                    else if (c == 27) { state = AppState::MENU; coll = coll_init; }
                    else { coll = coll_init; }
                }
            }
        }

        renderer.EndFrame();
    }

    return 0;
}
