#include "Application.h"
using namespace ST;

extern Application* CreateApplication();

int main(int argc, char* argv[]){
    Application* app = CreateApplication();
    app->Init();
    float cachedTime = 0;
    bool  hasCached  = false;
    while(!app->ShouldClose()){
        if(hasCached == false){
            hasCached  = true;
            cachedTime = app->GetAPPCurrentTime();
        }
        else{
            float currentTime = app->GetAPPCurrentTime();
            float deltaTime   = currentTime - cachedTime;
            cachedTime        = currentTime;
            app->Tick(deltaTime);
            app->Render(deltaTime);
        }
    }
    app->Destroy();
    delete app;
}
