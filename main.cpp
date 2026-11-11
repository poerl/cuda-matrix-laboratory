#include "MainView.h"
#include "Model.h"
#include "Presenter.h"
#include <memory>
#include <iostream>
#include <qapplication.h>

int main(int argc, char* argv[]) {
    QApplication app{argc, argv};

    auto model = std::make_unique<Model>();
    std::cout << model->getComputerName() << std::endl;
    auto view = std::make_unique<MainView>("Well");

    auto present = std::make_unique<Presenter>(model.get(), view.get());
    present->show();
    
    return app.exec();
}