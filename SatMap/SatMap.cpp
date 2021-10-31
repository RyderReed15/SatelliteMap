// SatMap.cpp : This file contains the 'main' function. Program execution begins and ends there.
//







#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGoogleMap.h>
#include <Wt/WStringListModel.h>
#include <Wt/WTemplate.h>
#include <Wt/WPanel.h>
#include <Wt/WGroupBox.h>

#include <Wt/WSpinBox.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>
#include <Wt/WSlider.h>
#include <Wt/WTimer.h>
#include <Wt/WPaintDevice.h>
#include <Wt/WCircleArea.h>
#include <Wt/WCssDecorationStyle.h>

#include "Utils.h"
#include "InfoRetriever.h"





std::vector<Eci> positions;

Wt::Coordinates deltaHelper = { 0, 0 };



class Application : public Wt::WApplication {
public:
	Application(const Wt::WEnvironment& env);
private:

};

class SatContainer : public Wt::WGroupBox {
public:
	SatContainer(Tle tleData, SGP4 satData) : Wt::WGroupBox(), tleData(tleData), sgp4Data(satData) {
		setWidth(400);
		setTitle(tleData.Name());
		
		Height = addNew<Wt::WText>("<p>Height: " + std::to_string(Utils::GetAltitude(satData)) + " km </p>");
		Velocity = addNew<Wt::WText>("<p>Velocity: " + std::to_string(Utils::GetVelocity(satData)) + " km/s </p>");
		Number = addNew<Wt::WText>("<p>Orbit Number: " + std::to_string(tleData.OrbitNumber()) + " </p>");
		PerDay = addNew<Wt::WText>("<p>Orbits Per Day: " + std::to_string(tleData.MeanMotion()) + " </p>");
		Inc = addNew<Wt::WText>("<p>Orbit Inclination: " + std::to_string(tleData.Inclination(true)) + " degrees </p>");
		Eccen = addNew<Wt::WText>("<p>Orbit Eccentricity: " + std::to_string(tleData.Eccentricity()) + " </p>");
		FindFlyOver = addNew<Wt::WPushButton>("Find Next Viewing Opportunity");
		addNew<Wt::WBreak>();
		addNew<Wt::WText>(" Long: ");
		Long = addNew<Wt::WLineEdit>("39.25568");
		Long->setWidth(70);
		addNew<Wt::WText>("Lat: ");
		Lat = addNew<Wt::WLineEdit>("-76.7131561");
		Lat->setWidth(70);
		addNew<Wt::WText>(" VisCone: ");
		VisCone = addNew<Wt::WSpinBox>();
		addNew<Wt::WBreak>();
		Time = addNew<Wt::WText>("");
		VisCone->setRange(0, 90);
		VisCone->setWidth(30);
		VisCone->setValue(25);
		FindFlyOver->clicked().connect([=] {
			DateTime time = Utils::FindVisibleTime(DateTime::Now(), tleData, satData, Observer(strtod(Lat->text().toUTF8().c_str(), 0), strtod(Long->text().toUTF8().c_str(), 0), 0.5), VisCone->value());
			Time->setText("Next Visible Time: " + time.ToString());
			});

	}
	void updateData(Tle tle, SGP4 sat) {
		tleData = tle;
		sgp4Data = sat;
		Height->setText("<p>Height: " + std::to_string(Utils::GetAltitude(sgp4Data)) + " km </p>");
		Velocity->setText("<p>Velocity: " + std::to_string(Utils::GetVelocity(sgp4Data)) + " km/s </p>");
		Number->setText("<p>Orbit Number: " + std::to_string(tleData.OrbitNumber()) + " </p>");
		PerDay->setText("<p>Orbits Per Day: " + std::to_string(tleData.MeanMotion()) + " </p>");
		Inc->setText("<p>Orbit Inclination: " + std::to_string(tleData.Inclination(true)) + " degrees </p>");
		Eccen->setText("<p>Orbit Eccentricity: " + std::to_string(tleData.Eccentricity()) + " </p>");
	}

	Tle tleData;
	SGP4 sgp4Data;
private:
	Wt::WText* Height;
	Wt::WText* Velocity;
	Wt::WText* Number;
	Wt::WText* PerDay;
	Wt::WText* Inc;
	Wt::WText* Eccen;
	Wt::WPushButton* FindFlyOver;
	Wt::WLineEdit* Lat;
	Wt::WLineEdit* Long;
	Wt::WSpinBox* VisCone;
	Wt::WText* Time;
};

class Painted : public Wt::WPaintedWidget
{
public:
	Painted()
		: WPaintedWidget(), zoom(.1)
	{
		
		resize(1800, 920);   // Provide a default size.
		selectedId = 0;
	}
	void setId(int id) {
		selectedId = id;
	}
	void setZoom(double z) {
		zoom = z;
			update(); 
	}
	void increaseZoom(double z) {
		zoom *= 1+z;
			update(); 
	}
	void decreaseZoom(double z) {
		zoom /= 1+z;
			update();
	}
	void setData(std::vector<Eci>& pdata) {
		data = pdata;
		
			update(); 
      // Trigger a repaint.
	}
	void panCamera(Wt::Coordinates coords) {
		camera.x -= coords.x / 3.f * DEG_2_RAD;
		camera.y -= coords.y / 3.f * DEG_2_RAD;
		if (camera.x > 2 * PI) {
			camera.x -= 2 * PI;
		}
		if (camera.x < 0) {
			camera.x += 2 * PI;
		}
		if (camera.y > 2 * PI) {
			camera.y -= 2 * PI;
		}
		if (camera.y < 0) {
			camera.y += 2 * PI;
		}
		update();
	}
	std::vector<Vector>& getPos() {
		return pos;
	}

protected:
	void paintEvent(Wt::WPaintDevice* paintDevice) {
		Wt::WPainter painter(paintDevice);
		painter.setBrush(Wt::WBrush(Wt::WColor(Wt::StandardColor::Blue)));
		double radius = zoom * EARTH_RADIUS;
		
		painter.drawEllipse(1800 / 2 - radius, 920 / 2 - radius, radius * 2, radius * 2);
		painter.setBrush(Wt::WBrush(Wt::WColor(Wt::StandardColor::Green)));
		for (int i = 0; i < data.size(); i++) {
			if (pos.size() <= i) {
				pos.push_back(Vector(0, 0, 0));
			}
			pos[i] = cameraTransform(data[i].Position().x, data[i].Position().z, data[i].Position().y);

			

			if (pos[i].z <= 2000 && (pos[i].z >= 0 || sqrt(pow(pos[i].x, 2) + pow(pos[i].y, 2)) > radius + 4)) {
				
				painter.drawEllipse(1800 / 2 - pos[i].x - 2.5, 920/2 - pos[i].y - 2.5, 5, 5);
				
			}
			if (i == selectedId) {
				//OrbitalElements orbElems(((SatContainer*)sats->currentWidget())->tleData);
				//double Apoapsis = orbElems.RecoveredSemiMajorAxis() * 2 - orbElems.Perigee();
				//painter.drawEllipse
			}

		}

	}
	

private:

	Vector cameraTransform(double x, double y, double z) {
		Vector transform;
		//View transform
		transform.x = zoom * (x * cos(camera.x) + z * sin(camera.x));
		transform.z = z * cos(camera.x) - x * sin(camera.x);
		transform.y = zoom * (y * cos(camera.y) + transform.z * sin(camera.y));
		transform.z = zoom * (transform.z * cos(camera.y) - y * sin(camera.y));

		return transform;
	}

	int selectedId;
	
	double zoom;
	Vector camera;
	std::vector<Eci> data;
	std::vector<Vector> pos;
	Wt::WStackedWidget* sats;
};



Application::Application(const Wt::WEnvironment& env) : Wt::WApplication(env){


	deltaHelper = { 1800 / 2, 920 / 2 };

	setTitle("Sat Map");


	root()->setContentAlignment(Wt::AlignmentFlag::Left);


	Painted* DrawSpace = root()->addWidget(std::make_unique<Painted>());
	auto panel = root()->addWidget(std::make_unique<Wt::WStackedWidget>());

	Wt::WCssDecorationStyle satInfoDecor;
	satInfoDecor.setBackgroundColor(Wt::WColor(32, 32, 32, 255));
	satInfoDecor.setForegroundColor(Wt::StandardColor::White);
	Wt::WCssDecorationStyle rootDecor;
	rootDecor.setBackgroundColor(Wt::WColor(32, 32, 32, 255));
	panel->setDecorationStyle(satInfoDecor);
	root()->setDecorationStyle(rootDecor);

	DrawSpace->setDecorationStyle(rootDecor);

	
	
	std::vector<Tle> allTles = Query::GetTleGroup("ACTIVE");
	std::vector<SGP4> allActiveSats = Query::GetSGP4Group(allTles);

	panel->addNew<Wt::WText>("Click A Sattelite to get Started!");
	for (int i = 0; i < allTles.size(); i++) {
		panel->addNew<SatContainer>(allTles[i], allActiveSats[i]);
	}

	
	DrawSpace->setZoom(.05);
	DrawSpace->setPositionScheme(Wt::PositionScheme::Absolute);

	for (int i = 0; i < allActiveSats.size(); i++) {
		positions.push_back((allActiveSats)[i].FindPosition(DateTime::Now()));
	}
	DrawSpace->setData(positions);

	panel->setPositionScheme(Wt::PositionScheme::Absolute);

	

	auto timer = root()->addChild(std::make_unique<Wt::WTimer>());
	timer->setInterval(std::chrono::seconds(2));
	timer->timeout().connect([=] {
		positions.clear();
		for (int i = 0; i < allActiveSats.size(); i++) {
			positions.push_back(allActiveSats[i].FindPosition(DateTime::Now()));
			if (i == panel->currentIndex()-1) {
				((SatContainer*)panel->currentWidget())->updateData(allTles[i], allActiveSats[i]);
			}
		}
		DrawSpace->setData(positions);
		
	});
	timer->start();

	DrawSpace->clicked().connect([=] (const Wt::WMouseEvent& e){
		std::vector<Vector> pos = DrawSpace->getPos();
		for (int i = 0; i < pos.size(); i++)
		{
			if (abs(e.widget().x - (1800 / 2 - pos[i].x)) < 9 && abs(e.widget().y - (920 / 2 - pos[i].y)) < 9) {
				panel->setCurrentIndex(i+1);
				break;
			}
		}
		});
	DrawSpace->mouseWheel().connect([=](const Wt::WMouseEvent& e) {
		if (e.wheelDelta() > 0) {
			DrawSpace->increaseZoom(.1 * e.wheelDelta());
		}
		else {
			DrawSpace->decreaseZoom(-.1 * e.wheelDelta());
		}
		});

	DrawSpace->mouseWentDown().connect([=](const Wt::WMouseEvent& e) {
		deltaHelper = e.widget();
		});

	DrawSpace->mouseDragged().connect([=](const Wt::WMouseEvent& e) {
		DrawSpace->panCamera({ e.widget().x - deltaHelper.x,e.widget().y - deltaHelper.y });
		deltaHelper = e.widget();
		});
	
	
	

}

int main(int argc, char** argv){

	return Wt::WRun(argc, argv, [](const Wt::WEnvironment& env) {
		return std::make_unique<Application>(env);
		});
	
}

