#include "Convexia.h"
#include "Dot.h"
#include "Triangulation.h"
#include "STLReader.h"
#include "STLWriter.h"
#include "OBJReader.h"
#include "OBJWriter.h"
#include "QuickHull.h"
#include "Operations.h"
#include <iostream>
#include <set>
#include <QFileDialog>
#include <QGridLayout>
#include <QStatusBar>
using namespace Geometry;
using namespace Algorithm;
using namespace std;

Convexia::Convexia(QWidget* parent) : QMainWindow(parent)
{
    setupUi();

    connect(loadFile, &QPushButton::clicked, this, &Convexia::onLoadClick);
}

Convexia::~Convexia()
{
}

void Convexia::onLoadClick()
{
    customStatusBar->showMessage("Select a file .");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.stl *.obj)"));

    if (!fileName.isEmpty())
    {
        inputFilePath = fileName;
        triangulation = readFile(inputFilePath);
        customStatusBar->showMessage("Loading file and Generating Convex Hull !.");
        OpenGlWidget::Data data = convertTrianglulationToGraphicsObject(triangulation);
        openglWidgetInput->setData(data);
        set<Dot> PointCloudSet;
        for (int i = 0; i < data.vertices.size(); i = i + 3) {
            Dot d(static_cast<double>(data.vertices[i]), static_cast<double>(data.vertices[i + 1]), static_cast<double>(data.vertices[i + 2]));

            PointCloudSet.insert(d);
        }
        vector<Dot>PointCloud(PointCloudSet.begin(), PointCloudSet.end());
        vector<Face> output = QuickHull::quickHull(PointCloud);
        OpenGlWidget::Data data1;

        for (Face f : output) {
            data1.vertices.push_back(static_cast<GLfloat>(f.D1().X()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D1().Y()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D1().Z()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D2().X()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D2().Y()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D2().Z()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D3().X()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D3().Y()));
            data1.vertices.push_back(static_cast<GLfloat>(f.D3().Z()));

            GVector nor = Operations::getNormal(f.D1(), f.D2(), f.D3());

            for (int i = 0;i < 3;i++) {
                data1.normals.push_back(static_cast<GLfloat>(nor.X()));
                data1.normals.push_back(static_cast<GLfloat>(nor.Y()));
                data1.normals.push_back(static_cast<GLfloat>(nor.Z()));
            }
        }
        openglWidgetOutput->setData(data1);
        customStatusBar->showMessage("Convex Hull Generated!.");
    }

}


void Convexia::setupUi()
{
    loadFile = new QPushButton("Load File", this);
    openglWidgetInput = new OpenGlWidget(this);
    openglWidgetOutput = new OpenGlWidget(this);
    progressbar = new QProgressBar(this);
    customStatusBar = new QStatusBar(this);
    graphicsSynchronizer = new GraphicsSynchronizer(openglWidgetInput, openglWidgetOutput);

    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(loadFile, 0, 0);
    layout->addWidget(progressbar, 0, 1, 1, 4);
    layout->addWidget(openglWidgetInput, 1,0,1,2);
    layout->addWidget(openglWidgetOutput, 1,2,1,2);
    layout->addWidget(customStatusBar, 2,0);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(layout);
    customStatusBar->setFixedHeight(30);

    customStatusBar->showMessage("Welcome to CONVEXIA APP ! Load your file.");
}

OpenGlWidget::Data Convexia::convertTrianglulationToGraphicsObject(const Triangulation& inTriangulation)
{
    OpenGlWidget::Data data;
    int count = 1;
    for (Triangle triangle : inTriangulation.triangles)
    {
        for (Point point : triangle.Points())
        {
            data.vertices.push_back(inTriangulation.uniqueNumbers[point.X()]);
            data.vertices.push_back(inTriangulation.uniqueNumbers[point.Y()]);
            data.vertices.push_back(inTriangulation.uniqueNumbers[point.Z()]);
        }

        Point normal = triangle.Normal();

        for (size_t i = 0; i < 3; i++)
        {
            data.normals.push_back(inTriangulation.uniqueNumbers[normal.X()]);
            data.normals.push_back(inTriangulation.uniqueNumbers[normal.Y()]);
            data.normals.push_back(inTriangulation.uniqueNumbers[normal.Z()]);
        }
        progressbar->setValue(count);
        count++;
    }
    return data;
}


Triangulation Convexia::readFile(const QString& filePath)
{
    Triangulation triangulation;
    if (filePath.endsWith(".stl", Qt::CaseInsensitive))
    {
        STLReader reader;
        reader.read(filePath.toStdString(), triangulation);
        progressbar->setRange(0, triangulation.triangles.size());
    }
    else if (filePath.endsWith(".obj", Qt::CaseInsensitive))
    {
        OBJReader reader;
        reader.read(filePath.toStdString(), triangulation);
        progressbar->setRange(0, triangulation.triangles.size());
    }

    return triangulation;
}

