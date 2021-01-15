#include "cell.h"

Cell::Cell()
{
    input_top = input_right = input_bottom = input_left = 0.0;
    input_top_next = input_right_next = input_bottom_next = input_left_next = 0.0;
    input_total = 0.0;

    output_top = output_right = output_bottom = output_left = 0.0;
    output_top_next = output_right_next = output_bottom_next = output_left_next = 0.0;
    output_total = output_total_next = 0.0;

    fluid_amount = 0.0;
    velocity = 0.0;
    startVelocity = 0.0;
    prevVelocity = 0.0;

    typeOfNeighbourhood = 0;
    typeOfNeighbourhoodOnSlant = 0;

    x = y = 0;

    fluid = source = boundary = outlet = outer_boundary = refactored = false;
}

Cell::Cell(bool fluid, bool outer)
{
    this->fluid = fluid;
    this->outer_boundary = outer;

    input_top = input_right = input_bottom = input_left = 0.0;
    input_top_next = input_right_next = input_bottom_next = input_left_next = 0.0;
    input_total = 0.0;

    output_top = output_right = output_bottom = output_left = 0.0;
    output_top_next = output_right_next = output_bottom_next = output_left_next = 0.0;
    output_total = output_total_next = 0.0;

    fluid_amount = 0.0;
    velocity = 0.0;
    startVelocity = 0.0;
    prevVelocity = 0.0;

    typeOfNeighbourhood = 0;
    typeOfNeighbourhoodOnSlant = 0;

    x = y = 0;

    source = boundary = outlet = refactored = false;
}

Cell::Cell(bool fluid, bool boundary, int x, int y)
{
    this->fluid = fluid;
    this->boundary = boundary;
    this->x = x;
    this->y = y;

    input_top = input_right = input_bottom = input_left = 0.0;
    input_top_next = input_right_next = input_bottom_next = input_left_next = 0.0;
    input_total = 0.0;

    output_top = output_right = output_bottom = output_left = 0.0;
    output_top_next = output_right_next = output_bottom_next = output_left_next = 0.0;
    output_total = output_total_next = 0.0;

    fluid_amount = 0.0;
    velocity = 0.0;
    startVelocity = 0.0;
    prevVelocity = 0.0;

    typeOfNeighbourhood = 0;
    typeOfNeighbourhoodOnSlant = 0;

    source = outlet = outer_boundary = refactored = false;
}

bool Cell::GetFluid()
{
    return fluid;
}

bool Cell::GetBoundary()
{
    return boundary;
}

bool Cell::GetSource()
{
    return source;
}

bool Cell::GetBalance()
{
    if( abs(velocity - prevVelocity) < 1e-5 )
        return true;
    return false;
}

bool Cell::GetOutlet()
{
    return outlet;
}

bool Cell::GetOuterBoundary()
{
    return outer_boundary;
}

double Cell::GetFluidAmount()
{
    return fluid_amount;
}

double Cell::GetVelocity()
{
    return velocity;
}

double Cell::GetPressure()
{
    double p = 0.5 * DENSITY * (startVelocity * startVelocity - this->GetVelocity() * this->GetVelocity() );
    return p;
}

double Cell::GetVelocityX()
{
    if(!refactored)
        return abs(input_left - input_right);
    return velocityX;
}

double Cell::GetVelocityY()
{
    if(!refactored)
        return abs(input_bottom - input_top);
    return velocityY;
}

int Cell::GetX()
{
    return x;
}

int Cell::GetY()
{
    return y;
}

void Cell::SetSource()
{
    source = true;
}

void Cell::SetOutlet()
{
    outlet = true;
}

void Cell::SetNeighbours(Cell* top, Cell* right, Cell* bottom, Cell* left)
{
    this->neighbours.insert({ "Top", top });
    this->neighbours.insert({ "Right", right });
    this->neighbours.insert({ "Bottom", bottom });
    this->neighbours.insert({ "Left", left });
}

void Cell::SetTypeOfNeighbourhood(int type)
{
    this->typeOfNeighbourhood = type;
}

void Cell::SetNeighboursOnSlant(Cell* topRight, Cell* bottomRight, Cell* bottomLeft, Cell* topLeft)
{
    this->neighboursOnSlant.insert({ "TopRight", topRight });
    this->neighboursOnSlant.insert({ "BottomRight", bottomRight });
    this->neighboursOnSlant.insert({ "BottomLeft", bottomLeft });
    this->neighboursOnSlant.insert({ "TopLeft", topLeft });
}

void Cell::SetTypeOfNeighbourhoodOnSlant(int type)
{
    this->typeOfNeighbourhoodOnSlant = type;
}

void Cell::FluidFlow()
{
    //calculate the coordinates of the output vector
    double x_direction = input_left - input_right;
    double y_direction = input_bottom - input_top;

    //save input velocity
    this->velocity = sqrt(x_direction * x_direction + y_direction * y_direction);

    //save total input
    this->input_total = input_left + input_bottom + input_right + input_top;

    //if velocity is 0, call UniformFlow()
    if( abs(this->velocity) < 1e-5 )
    {
        UniformFlow();
        return;
    }

    //calculate ox, oy and quadrant
    int ox = 0;
    int oy = 0;
    int quadrant = 0;

    //calculate ox
    if( abs(x_direction) > 1e-5 && x_direction > 0 && abs(y_direction) < 1e-5 )
        ox = 1;
    else if( abs(x_direction) > 1e-5 && x_direction < 0 && abs(y_direction) < 1e-5 )
        ox = -1;

    //calculate oy
    if( abs(y_direction) > 1e-5 && y_direction > 0 && abs(x_direction) < 1e-5 )
        oy = 1;
    else if( abs(y_direction) > 1e-5 && y_direction < 0 && abs(x_direction) < 1e-5 )
        oy = -1;

    //calculate quadrant
    if( ox == 0 && oy == 0 )
    {
        if( x_direction > 0 && y_direction > 0 )
            quadrant = 1;
        if( x_direction < 0 && y_direction > 0 )
            quadrant = 2;
        if( x_direction < 0 && y_direction < 0 )
            quadrant = 3;
        if( x_direction > 0 && y_direction < 0 )
            quadrant = 4;
    }

    //if quadrant != 0 calculate angle
    double angle = 0.0;
    double angle_r = 0.0;
    double abs_angle = 0.0;
    if( quadrant != 0 )
    {
        angle_r = atan(y_direction / x_direction);
        angle = angle_r * 180 / PI;
        abs_angle = abs(angle);
    }


    //calculate output direction (it is used for typeOfNeighbourhood != 0)
    int direction2 = ChooseDirection2(ox, oy, quadrant, angle);

    int n = this->typeOfNeighbourhood;
    int s = this->typeOfNeighbourhoodOnSlant;

    //calculate output direction (it is used for typeOfNeighbourhood == 0)
    int direction = ChooseDirection(x_direction, y_direction);

    double top_flow = 0.0;
    double right_flow = 0.0;
    double bottom_flow = 0.0;
    double left_flow = 0.0;

    double top_right_flow = 0.0;
    double bottom_right_flow = 0.0;
    double bottom_left_flow = 0.0;
    double top_left_flow = 0.0;

    double a = 0.0;

    if (typeOfNeighbourhood == 0)
    {
        switch (typeOfNeighbourhoodOnSlant)
        {
            case 0:
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5-a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant( top_right_flow*abs(sin(angle_r)), top_right_flow*cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a - 0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a - 0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow * abs(sin(angle_r)), bottom_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow * abs(sin(angle_r)), bottom_left_flow * cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a - 0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;

            case 1: //cannot move to top-right
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow * abs(sin(angle_r)), bottom_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow * abs(sin(angle_r)), bottom_left_flow * cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;

            case 2: //cannot move to bottom-right
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow* abs(sin(angle_r)), 0, 0, top_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow* abs(sin(angle_r)), 0, 0, top_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;


            case 3: //cannot move to bottom-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow* abs(sin(angle_r)), 0, 0, top_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow* abs(sin(angle_r)), 0, 0, top_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                }
                break;

            case 4: //cannot move to top-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;

            case 5: //cannot move to top-right and bottom-right
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow* abs(sin(angle_r)), 0, 0, top_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow * abs(sin(angle_r)), bottom_left_flow * cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    right_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;

            case 6: //cannot move to top-right and bottom-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow* abs(sin(angle_r)), 0, 0, top_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                }
                break;

            case 7: //cannot move to top-right and top-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow * abs(sin(angle_r)), bottom_left_flow * cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    top_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 3;
                    bottom_left_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;

            case 8: //cannot move to bottom-right and bottom-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 3;
                    top_left_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    bottom_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                }
                break;

            case 9: //cannot move to bottom-right and top-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;

            case 10:    //cannot move to bottom-left and top-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 3;
                    top_right_flow = this->velocity / 3;
                    bottom_right_flow = this->velocity / 3;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    left_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                break;

            case 11:    //cannot move to top-right, bottom-right and bottom-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_left_flow = this->velocity * a / 0.5;
                        top_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow * abs(sin(angle_r)), 0, 0, top_left_flow * cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        left_flow = this->velocity * a / 0.5;
                        top_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_left_flow* abs(sin(angle_r)), 0, 0, top_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_left_flow, 0, 0, 0, 8);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    bottom_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    right_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    top_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, top_left_flow, 8);
                }
                break;

            case 12:    //cannot move to bottom-right, bottom-left and top-left
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        top_right_flow = this->velocity * a / 0.5;
                        right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        top_flow = this->velocity * a / 0.5;
                        top_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(top_right_flow * abs(sin(angle_r)), top_right_flow * cos(angle_r), 0, 0, direction);
                    }
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    right_flow = this->velocity * cos(angle_r);
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    //diffusion
                    top_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(top_right_flow, 0, 0, 0, 2);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    bottom_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    top_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, top_right_flow, 0, 0, 2);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    left_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                break;

            case 13:    //cannot move to bottom-left, top-left and top-right
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    a = (angle + 90) / 90;
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_right_flow = this->velocity * a / 0.5;
                        bottom_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow * cos(angle_r), bottom_right_flow * abs(sin(angle_r)), 0, direction);
                    }
                    else
                    {
                        a -= 0.5;
                        right_flow = this->velocity * a / 0.5;
                        bottom_right_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, bottom_right_flow* cos(angle_r), bottom_right_flow* abs(sin(angle_r)), 0, direction);
                    }
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    top_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_right_flow, 0, 4);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    right_flow = this->velocity / 2;
                    bottom_right_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, bottom_right_flow, 0, 0, 4);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    left_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                break;

            case 14:    //cannot move to top-left, top-right and bottom-right
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    if (a < 0.5 || abs(a-0.5) < 1e-5)
                    {
                        bottom_left_flow = this->velocity * a / 0.5;
                        left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                    else
                    {
                        a -= 0.5;
                        bottom_flow = this->velocity * a / 0.5;
                        bottom_left_flow = this->velocity * (0.5 - a) / 0.5;
                        FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                        FlowToNeighboursOnSlant(0, 0, bottom_left_flow* abs(sin(angle_r)), bottom_left_flow* cos(angle_r), direction);
                    }
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    right_flow = this->velocity * cos(angle_r);
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    top_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    //diffusion
                    bottom_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, bottom_left_flow, 0, 6);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    right_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    //diffusion
                    left_flow = this->velocity / 2;
                    bottom_left_flow = this->velocity / 2;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                    FlowToNeighboursOnSlant(0, 0, 0, bottom_left_flow, 6);
                }
                break;

            case 15:    //cannot move to any of slant directions
                a = abs_angle / 90;
                if (angle > 0 && x_direction > 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    right_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction < 0 && y_direction > 0)
                {
                    top_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle > 0 && x_direction < 0 && y_direction < 0)
                {
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    left_flow = this->velocity * cos(angle_r);
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (angle < 0 && x_direction > 0 && y_direction < 0)
                {
                    right_flow = this->velocity * cos(angle_r);
                    bottom_flow = this->velocity * abs(sin(angle_r));
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction > 0)
                {
                    top_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (abs(x_direction) < 1e-5 && y_direction < 0)
                {
                    bottom_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction > 0 && abs(y_direction) < 1e-5)
                {
                    right_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                else if (x_direction < 0 && abs(y_direction) < 1e-5)
                {
                    left_flow = this->velocity;
                    FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                }
                break;
        }
    }
    else{

    //for typeOfNeighbourhood != 0
    switch(direction2)
    {
    //direction is top
    case 1:
        if(n==0 || n==2 || n==3 || n==4 || n==8 || n==9 || n==10 || n==12)
        {
            //diffusion
            if(s==0 || s==2 || s==3 || s==8)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(partialFlow, 0.0, 0.0, 0.0);
                FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, 0.0, 8);
                FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, 0.0, 2);
            }
            else if(s==1 || s==5 || s==6 || s==11)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(2*partialFlow, 0.0, 0.0, 0.0);
                FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, 0.0, 8);
            }
            else if(s==4 || s==9 || s==10 || s==12)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(2*partialFlow, 0.0, 0.0, 0.0);
                FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, 0.0, 2);
            }

        }
        else if(s==0 || s==2 || s==3 || s==8)
        {
            double partialFlow = this->velocity / (2 * sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else if(s==1 || s==5 || s==6 || s==11)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
        }
        else if(s==4 || s==9 || s==10 || s==12)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else if(n==1)
        {
            double partialFlow = this->velocity / 3;
            FlowToNeighbours(0.0, partialFlow, partialFlow, partialFlow);
        }
        else if(n==6)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours(0.0, partialFlow, 0.0, partialFlow);
        }
        else if(n==5 || n==11)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, 0.0, 0.0, partialFlow);
        }
        else if(n==7 || n==13)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, partialFlow, 0.0, 0.0);
        }
        else if(s==7)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(s==14)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(s==13)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, 0.0, partialFlow, 0.0);
        }
        break;

    //========================================================================
    //direction is top-right
    case 2:
        if(s==0 || s==2 || s==3 || s==4 || s==8 || s==9 || s==10 || s==12)
        {
            //diffusion
            if(n==0 || n==3 || n==4 || n==10)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 2);
                FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 1);
                FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 3);
            }
            else if(n==2 || n==8 || n==9 || n==12)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(2*partialFlow/sqrt(2), 2*partialFlow/sqrt(2), 0.0, 0.0, 2);
                FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 1);
            }
            else if(n==1 || n==6 || n==7 || n==13)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(2*partialFlow/sqrt(2), 2*partialFlow/sqrt(2), 0.0, 0.0, 2);
                FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 3);
            }
        }
        else if(n==0 || n==3 || n==4 || n==10)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 1);
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 3);
        }
        else if(n==2 || n==8 || n==9 || n==12)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 1);
        }
        else if(n==1 || n==6 || n==7 || n==13)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 3);
        }
        else if(s==1 || s==6)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(s==5 || s==11)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
        }
        else if(s==7 || s==13)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(n==5)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 7);
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 5);
        }
        else if(n==11)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 7);
        }
        else if(n==14)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 5);
        }
        else
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        break;

    //========================================================================
    //direction is right
    case 3:
        if(n==0 || n==1 || n==3 || n==4 || n==6 || n==7 || n==10 || n==13)
        {
            //diffusion
            if(s==0 || s==3 || s==4 || s==10)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, partialFlow, 0.0, 0.0);
                FlowToNeighboursOnSlant(0.0, partialFlow, 0.0, 0.0, 2);
                FlowToNeighboursOnSlant(0.0, partialFlow, 0.0, 0.0, 4);
            }
            else if(s==2 || s==8 || s==9 || s==12)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 2*partialFlow, 0.0, 0.0);
                FlowToNeighboursOnSlant(0.0, partialFlow, 0.0, 0.0, 2);
            }
            else if(s==1 || s==6 || s==7 || s==13)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 2*partialFlow, 0.0, 0.0);
                FlowToNeighboursOnSlant(0.0, partialFlow, 0.0, 0.0, 4);
            }

        }
        else if(s==0 || s==3 || s==4 || s==10)
        {
            double partialFlow = this->velocity / (2 * sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(s==2 || s==8 || s==9 || s==12)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else if(s==1 || s==6 || s==7 || s==13)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(n==2)
        {
            double partialFlow = this->velocity / 3;
            FlowToNeighbours(partialFlow, 0.0, partialFlow, partialFlow);
        }
        else if(n==9)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours(partialFlow, 0.0, partialFlow, 0.0);
        }
        else if(n==8 || n==12)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(partialFlow, 0.0, 0.0, 0.0);
        }
        else if(n==5 || n==14)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, 0.0, partialFlow, 0.0);
        }
        else if(s==5)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(s==11)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
        }
        else if(s==14)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, 0.0, 0.0, partialFlow);
        }
        break;

    //========================================================================
    //direction is bottom-right
    case 4:
        if(s==0 || s==1 || s==3 || s==4 || s==6 || s==7 || s==10 || s==13)
        {
            //diffusion
            if(n==0 || n==1 || n==4 || n==7)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 4);
                FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 3);
                FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 5);
            }
            else if(n==3 || n==6 || n==10 || n==13)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(0.0, 2*partialFlow/sqrt(2), 2*partialFlow/sqrt(2), 0.0, 4);
                FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 3);
            }
            else if(n==2 || n==5 || n==9 || n==14)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(0.0, 2*partialFlow/sqrt(2), 2*partialFlow/sqrt(2), 0.0, 4);
                FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 5);
            }
        }
        else if(n==0 || n==1 || n==4 || n==7)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 3);
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 5);
        }
        else if(n==3 || n==6 || n==10 || n==13)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 3);
        }
        else if(n==2 || n==5 || n==9 || n==14)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 5);
        }
        else if(s==2 || s==9)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(s==8 || s==12)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else if(s==5 || s==14)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(n==8)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 1);
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 7);
        }
        else if(n==12)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 1);
        }
        else if(n==11)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 7);
        }
        else
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
        }
        break;

    //========================================================================
    //direction is bottom
    case 5:
        if(n==0 || n==1 || n==2 || n==4 || n==5 || n==7 || n==9 || n==14)
        {
            //diffusion
            if(s==0 || s==1 || s==4 || s==7)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 0.0, partialFlow, 0.0);
                FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, 0.0, 6);
                FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, 0.0, 4);
            }
            else if(s==2 || s==5 || s==9 || s==14)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 0.0, 2*partialFlow, 0.0);
                FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, 0.0, 6);
            }
            else if(s==3 || s==6 || s==10 || s==13)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 0.0, 2*partialFlow, 0.0);
                FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, 0.0, 4);
            }
        }
        else if(s==0 || s==1 || s==4 || s==7)
        {
            double partialFlow = this->velocity / (2 * sqrt(2));
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(s==2 || s==5 || s==9 || s==14)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(s==3 || s==6 || s==10 || s==13)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(n==3)
        {
            double partialFlow = this->velocity / 3;
            FlowToNeighbours(partialFlow, partialFlow, 0.0, partialFlow);
        }
        else if(n==6)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours(0.0, partialFlow, 0.0, partialFlow);
        }
        else if(n==8 || n==11)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, 0.0, 0.0, partialFlow);
        }
        else if(n==10 || n==13)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, partialFlow, 0.0, 0.0);
        }
        else if(s==8)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else if(s==11)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
        }
        else if(s==12)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(partialFlow, 0.0, 0.0, 0.0);
        }
        break;

    //========================================================================
    //direction is bottom-left
    case 6:
        if(s==0 || s==1 || s==2 || s==4 || s==5 || s==7 || s==9 || s==14)
        {
            //diffusion
            if(n==0 || n==1 || n==2 || n==5)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 6);
                FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 5);
                FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 7);
            }
            else if(n==3 || n==6 || n==8 || n==11)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(0.0, 0.0, 2*partialFlow/sqrt(2), 2*partialFlow/sqrt(2), 6);
                FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 7);
            }
            else if(n==4 || n==7 || n==9 || n==14)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(0.0, 0.0, 2*partialFlow/sqrt(2), 2*partialFlow/sqrt(2), 6);
                FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 5);
            }
        }
        else if(n==0 || n==1 || n==2 || n==5)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 5);
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 7);
        }
        else if(n==3 || n==6 || n==8 || n==11)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 7);
        }
        else if(n==4 || n==7 || n==9 || n==14)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, 0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 5);
        }
        else if(s==3 || s==6)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(s==8 || s==11)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
        }
        else if(s==10 || s==13)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(n==10)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 1);
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 3);
        }
        else if(n==12)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 1);
        }
        else if(n==13)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 0.0, 3);
        }
        else
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        break;

    //========================================================================
    //direction is left
    case 7:
        if(n==0 || n==1 || n==2 || n==3 || n==5 || n==6 || n==8 || n==11)
        {
            //diffusion
            if(s==0 || s==1 || s==2 || s==5)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 0.0, 0.0, partialFlow);
                FlowToNeighboursOnSlant(0.0, 0.0, 0.0, partialFlow, 8);
                FlowToNeighboursOnSlant(0.0, 0.0, 0.0, partialFlow, 6);
            }
            else if(s==3 || s==6 || s==8 || s==11)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 0.0, 0.0, 2*partialFlow);
                FlowToNeighboursOnSlant(0.0, 0.0, 0.0, partialFlow, 8);
            }
            else if(s==4 || s==7 || s==9 || s==14)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighbours(0.0, 0.0, 0.0, 2*partialFlow);
                FlowToNeighboursOnSlant(0.0, 0.0, 0.0, partialFlow, 6);
            }
        }
        else if(s==0 || s==1 || s==2 || s==5)
        {
            double partialFlow = this->velocity / (2 * sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(s==3 || s==6 || s==8 || s==11)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, 0.0, 0.0, partialFlow, 8);
        }
        else if(s==4 || s==7 || s==9 || s==14)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(n==4)
        {
            double partialFlow = this->velocity / 3;
            FlowToNeighbours(partialFlow, partialFlow, partialFlow, 0.0);
        }
        else if(n==9)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours(partialFlow, 0.0, partialFlow, 0.0);
        }
        else if(n==10 || n==12)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(partialFlow, 0.0, 0.0, 0.0);
        }
        else if(n==7 || n==13)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, 0.0, partialFlow, 0.0);
        }
        else if(s==10)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else if(s==12)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else if(s==13)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        else
        {
            double partialFlow = this->velocity;
            FlowToNeighbours(0.0, partialFlow, 0.0, 0.0);
        }
        break;

    //========================================================================
    //direction is bottom-left
    case 8:
        if(s==0 || s==1 || s==2 || s==3 || s==5 || s==6 || s==8 || s==11)
        {
            //diffusion
            if(n==0 || n==2 || n==3 || n==8)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 8);
                FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 1);
                FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 7);
            }
            else if(n==4 || n==9 || n==10 || n==12)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(2*partialFlow/sqrt(2), 0.0, 0.0, 2*partialFlow/sqrt(2), 8);
                FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 1);
            }
            else if(n==1 || n==5 || n==6 || n==11)
            {
                double partialFlow = this->velocity / 3;
                FlowToNeighboursOnSlant(2*partialFlow/sqrt(2), 0.0, 0.0, 2*partialFlow/sqrt(2), 8);
                FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 7);
            }
        }
        else if(n==0 || n==2 || n==3 || n==8)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 1);
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 7);
        }
        else if(n==4 || n==9 || n==10 || n==12)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 1);
        }
        else if(n==1 || n==5 || n==6 || n==11)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(partialFlow/sqrt(2), 0.0, 0.0, partialFlow/sqrt(2), 7);
        }
        else if(s==4 || s==9)
        {
            double partialFlow = this->velocity / (2*sqrt(2));
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(s==10 || s==12)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(partialFlow, partialFlow, 0.0, 0.0, 2);
        }
        else if(s==7 || s==14)
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, 0.0, partialFlow, partialFlow, 6);
        }
        else if(n==7)
        {
            double partialFlow = this->velocity / 2;
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 5);
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 3);
        }
        else if(n==13)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 3);
        }
        else if(n==14)
        {
            double partialFlow = this->velocity;
            FlowToNeighbours2(0.0, partialFlow/sqrt(2), partialFlow/sqrt(2), 0.0, 5);
        }
        else
        {
            double partialFlow = this->velocity / sqrt(2);
            FlowToNeighboursOnSlant(0.0, partialFlow, partialFlow, 0.0, 4);
        }
        break;

    }
    }

}

void Cell::UniformFlow()	//if vectors reduce itself, this function is performed
{
    double top_flow = 0.0;
    double right_flow = 0.0;
    double bottom_flow = 0.0;
    double left_flow = 0.0;

    double top_right_flow = 0.0;
    double bottom_right_flow = 0.0;
    double bottom_left_flow = 0.0;
    double top_left_flow = 0.0;

    switch (typeOfNeighbourhood)
    {
        case 0:	//(t && r && b && l)
            if (typeOfNeighbourhoodOnSlant == 0)
            {
                top_flow = this->input_total / 8;
                right_flow = this->input_total / 8;
                bottom_flow = this->input_total / 8;
                left_flow = this->input_total / 8;

                top_right_flow = this->input_total / 8;
                bottom_right_flow = this->input_total / 8;
                bottom_left_flow = this->input_total / 8;
                top_left_flow = this->input_total / 8;

                FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
                FlowToNeighboursOnSlant(top_right_flow/sqrt(2), top_right_flow/sqrt(2), 0, 0, 2);
                FlowToNeighboursOnSlant(0, bottom_right_flow / sqrt(2), bottom_right_flow / sqrt(2), 0, 4);
                FlowToNeighboursOnSlant(0, 0, bottom_left_flow / sqrt(2), bottom_left_flow / sqrt(2), 6);
                FlowToNeighboursOnSlant(top_left_flow / 2, 0, 0, top_left_flow / 2, 8);
            }
            else
            {
                top_flow = 0.25 * this->input_total;
                right_flow = 0.25 * this->input_total;
                bottom_flow = 0.25 * this->input_total;
                left_flow = 0.25 * this->input_total;
                FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            }
            break;

        case 1: //(!t && r && b && l)
            top_flow = 0.0;
            right_flow = this->input_total / 3;
            bottom_flow = this->input_total / 3;
            left_flow = this->input_total / 3;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 2: //(t && !r && b && l)
            top_flow = this->input_total / 3;
            right_flow = 0.0;
            bottom_flow = this->input_total / 3;
            left_flow = this->input_total / 3;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 3: //(t && r && !b && l)
            top_flow = this->input_total / 3;
            right_flow = this->input_total / 3;
            bottom_flow = 0.0;
            left_flow = this->input_total / 3;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 4: //(t && r && b && !l)
            top_flow = this->input_total / 3;
            right_flow = this->input_total / 3;
            bottom_flow = this->input_total / 3;
            left_flow = 0.0;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 5: //(!t && !r && b && l)
            top_flow = 0.0;
            right_flow = 0.0;
            bottom_flow = this->input_total / 2;
            left_flow = this->input_total / 2;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 6: //(!t && r && !b && l)
            top_flow = 0.0;
            right_flow = this->input_total / 2;
            bottom_flow = 0.0;
            left_flow = this->input_total / 2;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 7: //(!t && r && b && !l)
            top_flow = 0.0;
            right_flow = this->input_total / 2;
            bottom_flow = this->input_total / 2;
            left_flow = 0.0;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 8: //(t && !r && !b && l)
            top_flow = this->input_total / 2;
            right_flow = 0.0;
            bottom_flow = 0.0;
            left_flow = this->input_total / 2;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 9: //(t && !r && b && !l)
            top_flow = this->input_total / 2;
            right_flow = 0.0;
            bottom_flow = this->input_total / 2;
            left_flow = 0.0;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 10: //(t && r && !b && !l)
            top_flow = this->input_total / 2;
            right_flow = this->input_total / 2;
            bottom_flow = 0.0;
            left_flow = 0.0;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 11: //(!t && !r && !b && l)
            top_flow = 0.0;
            right_flow = 0.0;
            bottom_flow = 0.0;
            left_flow = this->input_total;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 12: //(t && !r && !b && !l)
            top_flow = this->input_total;
            right_flow = 0.0;
            bottom_flow = 0.0;
            left_flow = 0.0;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 13: //(!t && r && !b && !l)
            top_flow = 0.0;
            right_flow = this->input_total;
            bottom_flow = 0.0;
            left_flow = 0.0;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;

        case 14: //(!t && !r && b && !l)
            top_flow = 0.0;
            right_flow = 0.0;
            bottom_flow = this->input_total;
            left_flow = 0.0;
            FlowToNeighbours(top_flow, right_flow, bottom_flow, left_flow);
            break;
    }

}

void Cell::FlowToNeighbours(double top_flow, double right_flow, double bottom_flow, double left_flow)
{
    neighbours["Right"]->SetLeftInput(right_flow);
    neighbours["Bottom"]->SetTopInput(bottom_flow);
    neighbours["Left"]->SetRightInput(left_flow);
    neighbours["Top"]->SetBottomInput(top_flow);
    output_right_next = right_flow;
    output_bottom_next = bottom_flow;
    output_left_next = left_flow;
    output_top_next = top_flow;
    output_total_next += output_bottom_next + output_left_next + output_right_next + output_top_next;
}

void Cell::FlowToNeighbours2(double top_flow, double right_flow, double bottom_flow, double left_flow, int direction)
{
    switch (direction)
    {
        case 1:
            this->neighbours["Top"]->SetBottomInput(top_flow);
            this->neighbours["Top"]->SetLeftInput(right_flow);
            this->neighbours["Top"]->SetRightInput(left_flow);
            break;

        case 3:
            neighbours["Right"]->SetLeftInput(right_flow);
            neighbours["Right"]->SetBottomInput(top_flow);
            neighbours["Right"]->SetTopInput(bottom_flow);
            break;

        case 5:
            neighbours["Bottom"]->SetTopInput(bottom_flow);
            neighbours["Bottom"]->SetLeftInput(right_flow);
            neighbours["Bottom"]->SetRightInput(left_flow);
            break;

        case 7:
            neighbours["Left"]->SetRightInput(left_flow);
            neighbours["Left"]->SetTopInput(bottom_flow);
            neighbours["Left"]->SetBottomInput(top_flow);
            break;
    }
    output_right_next = right_flow;
    output_bottom_next = bottom_flow;
    output_left_next = left_flow;
    output_top_next = top_flow;
    output_total_next += output_bottom_next + output_left_next + output_right_next + output_top_next;
}

void Cell::FlowToNeighboursOnSlant(double top_flow, double right_flow, double bottom_flow, double left_flow, int direction)
{
    switch (direction)
    {
        case 2:
            this->neighboursOnSlant["TopRight"]->SetLeftInput(right_flow);
            this->neighboursOnSlant["TopRight"]->SetBottomInput(top_flow);
            break;

        case 4:
            this->neighboursOnSlant["BottomRight"]->SetLeftInput(right_flow);
            this->neighboursOnSlant["BottomRight"]->SetTopInput(bottom_flow);
            break;

        case 6:
            this->neighboursOnSlant["BottomLeft"]->SetRightInput(left_flow);
            this->neighboursOnSlant["BottomLeft"]->SetTopInput(bottom_flow);
            break;

        case 8:
            this->neighboursOnSlant["TopLeft"]->SetRightInput(left_flow);
            this->neighboursOnSlant["TopLeft"]->SetBottomInput(top_flow);
            break;
    }
    output_right_next = right_flow;
    output_bottom_next = bottom_flow;
    output_left_next = left_flow;
    output_top_next = top_flow;
    output_total_next += output_bottom_next + output_left_next + output_right_next + output_top_next;
}

void Cell::SetTopInput(double input)
{
    this->input_top_next += input;
}

void Cell::SetRightInput(double input)
{
    this->input_right_next += input;
}

void Cell::SetBottomInput(double input)
{
    this->input_bottom_next += input;
}

void Cell::SetLeftInput(double input)
{
    this->input_left_next += input;
}

void Cell::SetStartVelocity(double v)
{
    this->startVelocity = v;
}

void Cell::Update()
{
    input_top = input_top_next;
    input_right = input_right_next;
    input_bottom = input_bottom_next;
    input_left = input_left_next;

    input_total = input_top + input_right + input_bottom + input_left;
    fluid_amount = input_total;
    prevVelocity = velocity;

    output_top = output_top_next;
    output_right = output_right_next;
    output_bottom = output_bottom_next;
    output_left = output_left_next;

    output_total = output_total_next;

    input_top_next = input_right_next = input_bottom_next = input_left_next = 0.0;
    output_top_next = output_right_next = output_bottom_next = output_left_next = 0.0;
    output_total_next = 0.0;
}

int Cell::ChooseDirection(double x_direction, double y_direction)
{
    if( abs(x_direction) < 1e-5 && y_direction > 0 )	//top
        return 1;
    if (x_direction > 0 && y_direction > 0)		//top right
        return 2;
    if (x_direction > 0 && abs(y_direction) < 1e-5)	//right
        return 3;
    if (x_direction > 0 && y_direction < 0)		//bottom right
        return 4;
    if (abs(x_direction) < 1e-5 && y_direction < 0)	//bottom
        return 5;
    if (x_direction < 0 && y_direction < 0)		//bottom left
        return 6;
    if (x_direction < 0 && abs(y_direction) < 1e-5)	//left
        return 7;
    if (x_direction < 0 && y_direction > 0)		//top left
        return 8;

    return 0;
}

int Cell::ChooseDirection2(int ox, int oy, int quadrant, double angle)
{
    if(oy == 1)
        return 1;   //top
    if(quadrant == 1)
    {
        if( angle > 67.5 )
            return 1;
        if( angle < 22.5 )
            return 3;
        return 2;   //top-right
    }
    if(ox == 1)
        return 3;   //right
    if(quadrant == 4)
    {
        if( angle < -67.5 )
            return 5;
        if( angle > -22.5 )
            return 3;
        return 4;   //bottom-right
    }
    if(oy == -1)
        return 5;   //bottom
    if(quadrant == 3)
    {
        if( angle > 67.5 )
            return 5;
        if( angle < 22.5 )
            return 7;
        return 6;   //bottom-left
    }
    if(ox == -1)
        return 7;   //left
    if(quadrant == 2)
    {
        if( angle < -67.5 )
            return 1;
        if( angle > -22.5 )
            return 7;
        return 8;   //top-left
    }

    return 0;
}


void Cell::GetMeanFromNeighbours(double sigma)
{
    double sum = 0.0;
    double mean = 0.0;
    double temp = 0.0;
    int nNeighbours = 0;

    if (neighbours["Top"]->GetFluid() && !neighbours["Top"]->GetOuterBoundary())
    {
        temp = neighbours["Top"]->GetVelocity();
        if( temp < sigma )
        {
            sum += temp;
            nNeighbours++;
        }
    }
    if (neighbours["Right"]->GetFluid() && !neighbours["Right"]->GetOuterBoundary())
    {
        temp = neighbours["Right"]->GetVelocity();
        if (temp < sigma)
        {
            sum += temp;
            nNeighbours++;
        }
    }
    if (neighbours["Bottom"]->GetFluid() && !neighbours["Bottom"]->GetOuterBoundary())
    {
        temp = neighbours["Bottom"]->GetVelocity();
        if (temp < sigma)
        {
            sum += temp;
            nNeighbours++;
        }
    }
    if (neighbours["Left"]->GetFluid() && !neighbours["Left"]->GetOuterBoundary())
    {
        temp = neighbours["Left"]->GetVelocity();
        if (temp < sigma)
        {
            sum += temp;
            nNeighbours++;
        }
    }
    if (neighboursOnSlant["TopRight"]->GetFluid() && !neighboursOnSlant["TopRight"]->GetOuterBoundary())
    {
        temp = neighboursOnSlant["TopRight"]->GetVelocity();
        if (temp < sigma)
        {
            sum += temp;
            nNeighbours++;
        }
    }
    if (neighboursOnSlant["BottomRight"]->GetFluid() && !neighboursOnSlant["BottomRight"]->GetOuterBoundary())
    {
        temp = neighboursOnSlant["BottomRight"]->GetVelocity();
        if (temp < sigma)
        {
            sum += temp;
            nNeighbours++;
        }
    }
    if (neighboursOnSlant["BottomLeft"]->GetFluid() && !neighboursOnSlant["BottomLeft"]->GetOuterBoundary())
    {
        temp = neighboursOnSlant["BottomLeft"]->GetVelocity();
        if (temp < sigma)
        {
            sum += temp;
            nNeighbours++;
        }
    }
    if (neighboursOnSlant["TopLeft"]->GetFluid() && !neighboursOnSlant["TopLeft"]->GetOuterBoundary())
    {
        temp = neighboursOnSlant["TopLeft"]->GetVelocity();
        if (temp < sigma)
        {
            sum += temp;
            nNeighbours++;
        }
    }

    mean = sum / nNeighbours;

    double vx = this->GetVelocityX();
    double vy = this->GetVelocityY();
    double f = velocity / mean;

    this->velocityX = vx / f;
    this->velocityY = vy / f;
    this->velocity = mean;

    this->refactored = true;
}
