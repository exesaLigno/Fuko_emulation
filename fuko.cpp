#include <fstream>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include "TXLib.h"
#include <assert.h>


#define DEBUG printf (">Debug output from %d line\n", __LINE__);


struct cplanet
{
    char name [20] = {};    //! Name of planet
    double mass = 0;        //! Mass of planet in <kilograms>
    double radius = 0;      //! Radius of planet in <meters>
    double day = 0;         //! Length of day on planet in <seconds>
};


namespace phys
{
    const double G = 6.67408e-11;
    const double pi = 3.14159265;
}


namespace settings
{
    int time_multiplier = 1;
}


namespace oscilator
{
    double osc_mass = 0;
    double osc_len = 0;
    double osc_ampl = 0;
}


cplanet init_planet     ();
long    file_size       (FILE* file);
double  get_gravity     (cplanet* planet);
#define gravity get_gravity (&planet)
#define _gravity get_gravity (planet)

double  get_angular_v   (cplanet* planet);
#define angular get_angular_v (&planet)
#define _angular get_angular_v (planet)

double  get_cycle_freq  (cplanet* planet);
#define cfreq get_cycle_freq (&planet)
#define _cfreq get_cycle_freq (planet)

int     draw_plot       (cplanet* planet, long max_time);





int main ()
{
    cplanet planet = init_planet ();
    printf ("%s: mass %g, radius %g, day %g\n", planet.name, planet.mass, planet.radius, planet.day);
    printf ("gravity on %s is %g, angular velocity is %g\n", planet.name, gravity, angular);
    draw_plot (&planet, 8000);
    return 0;
}




cplanet init_planet ()
{
    const int name = 1;
    const int mass = 2;
    const int radius = 3;
    const int day = 4;
    const int osc_mass = 5;
    const int osc_len = 6;
    const int time_multiplier = 7;
    const int osc_ampl = 8;

    cplanet planet;

    FILE* config_file = fopen ("config.cfg", "r");
    if (!config_file)
        assert (!"Configuration file not found!");

    long length = file_size (config_file);

    char* buffer = (char*) calloc (sizeof (char), length);
    length = fread (buffer, sizeof (char), length, config_file);

    char cmd [20] = {};
    int cmd_ind = 0;
    int parameter = 0;

    for (int counter = 0; counter < length; counter++)
    {
        if (!isspace (buffer [counter]))
            cmd [cmd_ind++] = buffer [counter];

        if (isspace (buffer [counter]) || counter == length - 1)
        {
            cmd [cmd_ind] = '\0';
            cmd_ind = 0;

            if (parameter)
            {
                if (parameter == mass)
                    planet.mass = atof (cmd);

                else if (parameter == radius)
                    planet.radius = atof (cmd);

                else if (parameter == day)
                    planet.day = atof (cmd);

                else if (parameter == name)
                    strcpy (planet.name, cmd);

                else if (parameter == osc_mass)
                    oscilator::osc_mass = atof (cmd);

                else if (parameter == osc_len)
                    oscilator::osc_len = atof (cmd);

                else if (parameter == time_multiplier)
                    settings::time_multiplier = atoi (cmd);

                else if (parameter == osc_ampl)
                    oscilator::osc_ampl = atoi (cmd);

                else
                    assert (!"Unknown error!");

                parameter = 0;
            }

            else
            {
                if (!stricmp (cmd, "/name") || !stricmp (cmd, "/n"))
                    parameter = name;

                else if (!stricmp (cmd, "/mass") || !stricmp (cmd, "/m"))
                    parameter = mass;

                else if (!stricmp (cmd, "/radius") || !stricmp (cmd, "/rad") || !stricmp (cmd, "/r"))
                    parameter = radius;

                else if (!stricmp (cmd, "/day") || !stricmp (cmd, "/period") || !stricmp (cmd, "/t"))
                    parameter = day;

                else if (!stricmp (cmd, "/osc_mass") || !stricmp (cmd, "/om"))
                    parameter = osc_mass;

                else if (!stricmp (cmd, "/osc_len") || !stricmp (cmd, "/ol"))
                    parameter = osc_len;

                else if (!stricmp (cmd, "/multiplier") || !stricmp (cmd, "/mp"))
                    parameter = time_multiplier;

                else if (!stricmp (cmd, "/osc_ampl") || !stricmp (cmd, "/oa"))
                    parameter = osc_ampl;

                else
                    assert (!"Unknown parameter!");
            }
        }
    }

    return planet;
}

long file_size (FILE* file)
{
    fseek (file, 0, SEEK_END);
    long file_size = ftell (file);
    rewind (file);

    return file_size;
}


double get_gravity (cplanet* planet)
{
    double g = 0;
    g = (phys::G * planet -> mass) / (planet -> radius * planet -> radius);
    return g;
}


double get_angular_v (cplanet* planet)
{
    double omega = 0;
    omega = (2 * phys::pi) / (planet -> day);
    return omega;
}


double get_cycle_freq (cplanet* planet)
{
    double omega = 0;
    omega = sqrt (_gravity / oscilator::osc_len);
    return omega;
}


int draw_plot (cplanet* planet, long max_time)
{
    txCreateWindow (600, 600);

    txLine (30, 570, 30, 30);
    txLine (30, 30, 25, 40);
    txLine (30, 30, 35, 40);

    txLine (30, 570, 570, 570);
    txLine (570, 570, 560, 565);
    txLine (570, 570, 560, 575);

    txTextOut (297, 580, "0");
    txLine (300, 573, 300, 567);
    txTextOut (5, 295, "0");
    txLine (27, 300, 33, 300);

    txTextOut (560, 580, "x, ì");
    txTextOut (5, 5, "y, ì");

    double current_time = 0; //! In seconds!

    double prev_x = 0;
    double prev_y = 0;
    double cur_x = 0;
    double cur_y = 0;
    double cycle_freq = _cfreq;
    double angular_velocity = _angular;

    while (current_time < max_time)
    {
        double deflection = oscilator::osc_ampl * 100 * sin (cycle_freq * current_time);
        cur_x = cos (angular_velocity * current_time) * deflection - sin (angular_velocity * current_time) * deflection;
        cur_y = sin (angular_velocity * current_time) * deflection + cos (angular_velocity * current_time) * deflection;
        txLine (prev_x + 300, prev_y + 300, cur_x + 300, cur_y + 300);
        prev_x = cur_x;
        prev_y = cur_y;
        current_time += 0.025;
        Sleep (25 / settings::time_multiplier);
    }

    return 0;
}



//end
