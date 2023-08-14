#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <X11/Xlib.h>
#include <fstream>
#include <json/json.h>


struct ActionCorner {
    // min, max
    int x[2];
    int y[2];
    // system command
    std::string cmd = "";
};


int main(int argc, char* argv[]) {
    Display *display = XOpenDisplay(nullptr);

    if(argc < 2) {
        std::cerr << "Path config.json non trovato." << std::endl;
        return 1;
    }

    if (display == nullptr) {
        std::cerr << "Unable to open X display." << std::endl;
        return 1;
    }

    // /home/banana/s_scripts/config.json
    std::cout << argv[1] << std::endl;
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Errore durante l'apertura del file json" << std::endl;
        return 1;
    }

    Json::Value rootJson;
    file >> rootJson;

    const Json::Value active_corners = rootJson["active_corners"];
    int quanti = active_corners.size(); // Calcola la dimensione dell'array

    ActionCorner* ac = new ActionCorner[quanti];
    int index = 0;

    for (const auto& corner : active_corners) {
        const Json::Value range_x = corner["range_x_min_max"];
        const Json::Value range_y = corner["range_y_min_max"];

        ac[index].x[0] = range_x[0].asInt();
        ac[index].x[1] = range_x[1].asInt();
        ac[index].y[0] = range_y[0].asInt();
        ac[index].y[1] = range_y[1].asInt();
        ac[index].cmd = corner["command"].asString() + "&";
        index++;
    }

    Window root = DefaultRootWindow(display);
    XEvent event;
    bool found = false;

    std::string lastCmd = "";

    while (true) {
        XQueryPointer(display, root, &event.xbutton.root, &event.xbutton.window,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);

        found = false;
        for (int i = 0; i < quanti; i++) {
            if (event.xbutton.x_root >= ac[i].x[0] &&
                event.xbutton.x_root <= ac[i].x[1] &&
                event.xbutton.y_root >= ac[i].y[0] &&
                event.xbutton.y_root <= ac[i].y[1]) {
                
                // evitiamo di rilanciare l'ultimo comando se il mouse sta ancora sull'hot corner
                if(lastCmd != ac[i].cmd.c_str()) {
                    lastCmd = ac[i].cmd.c_str();
                    system(ac[i].cmd.c_str());
                }

                found = true;
                break;
            }
        }

        if (found) {
            //std::cout << "Mouse position: X=" << event.xbutton.x_root << ", Y=" << event.xbutton.y_root << std::endl;
            //std::cout.flush();
            usleep(500000);
            continue;
        }

        // reset history comando
        lastCmd = "";
        usleep(100000); // Pause for a short time (100ms) to avoid excessive output
    }

    XCloseDisplay(display);
    delete[] ac;
    return 0;
}
