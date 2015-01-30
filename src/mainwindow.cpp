#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QFileDialog>
#include <QPicture>
#include <QTimer>
#include <QtWebKitWidgets>
#include <ctime>


ChordProgression computeChordsWrapper(const std::string& ruleFile, const std::vector<std::string>& ruleNames, const MachineLearningOutput& ml, const Melody& m, bool bis);


MainWindow::MainWindow(QWidget *p) :
    QMainWindow(p),
    ui(new Ui::MainWindow),
    currentMelody(),
    currentChordProgression(),
    currentPlayer(),
    timer(),
    watcher(),future()
{
    ui->setupUi(this);
    connect(ui->actionPlay_Pause,SIGNAL(triggered()),this,SLOT(togglePlay()));

    QFile html;
    html.setFileName(":/rendering/renderer.html");
    html.open(QIODevice::ReadOnly);
    QString content = html.readAll();
    ui->PartitionRender->setHtml(content);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::togglePlay()
{
    if(ui->Play->isChecked()){
        on_Play_toggled(false);
        ui->Play->setChecked(false);
    }else{
        ui->Play->setChecked(true);
        on_Play_toggled(true);
    }
}


void MainWindow::updatePlayer()
{
    static unsigned lastMeasure = INT_MAX;
    if(currentPlayer.isFinished())
    {
        on_Stop_released();
    }
    else if(currentPlayer && currentChordProgression.size()!=0)
    {
        Time position = currentPlayer.getPosition();
        unsigned measure = static_cast<unsigned>(std::max((position.getNumber(Time::MIN)*static_cast<float>(currentPlayer.getBPM())/4.0),0.0));
        if(lastMeasure!=measure)
        {
            lastMeasure=measure;
            if(measure < currentChordProgression.size())
            {
                QString partitionChords = "<font size = \"40\">";
                for(unsigned i=0;i<currentChordProgression.size();i++)
                {
                    std::ostringstream output;
                    if(i==measure)
                        partitionChords += "<font color = \"red\"><a name=\"current\">";
                    output << currentChordProgression[i];
                    partitionChords += QString::fromStdString(output.str())+" ";
                    if(i==measure)
                        partitionChords += "</a></font>";

                }
                partitionChords+="</font>";
                ui->Chords->setHtml(partitionChords);
                ui->Chords->scrollToAnchor("current");
            }
        }
    }
    else
        lastMeasure=INT_MAX;
}

void MainWindow::on_ImportABC_released()
{
    if(future.isRunning()){
        QMessageBox messageBox;
        messageBox.setWindowTitle("Computation in progress");
        messageBox.setText("A chord progression is currently being computed.\nPlease wait the end of the computation to open another file");
        messageBox.exec();
        return;
    }
    try
    {
        // open the file
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),"examples",tr("Abc (*.abc)\n All Files (*.*) "));
        std::string filePathStdString = filePath.toStdString();
        if (filePathStdString.size() != 0){
            currentMelody = readMelody(getFile(filePathStdString));
            currentChordProgression= ChordProgression();
            initialize_Player();

            QFileInfo fileInfo(filePath);
            QString fileName = fileInfo.fileName();
            ui->labelFileName->setText(fileName);

            // undisplay the chords
            ui->Chords->setText("");

            // display
            std::ostringstream output;
            output << currentMelody;
            QString partition = QString::fromStdString(output.str());
            ui->Partition->setText(partition);

            //Partition rendering
            QFile file;
            file.setFileName(filePath);
            file.open(QIODevice::ReadOnly);
            QString Abc = file.readAll();
            Abc.replace(QRegExp("[\r\n]"),"\\n");
            Abc.replace("\\n\\n","\\n");
            Abc.replace("'","\\'");
            file.close();

            ui->PartitionRender->page()->mainFrame()->evaluateJavaScript("ABCJS.renderAbc('notation', '"+Abc+"');");
            timer.setInterval(20);
            connect(&timer, SIGNAL(timeout()), this, SLOT(updatePlayer()));
            timer.start();
            ui->GenerateChords->setEnabled(true);
        }

    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_ImportABC_released");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_InportABC_released unknown");
        messageBox.exec();
    }
}

ChordProgression computeChordsWrapper(const std::string& ruleFile, const std::vector<std::string>& ruleNames, const MachineLearningOutput& ml, const Melody& m, bool bis)
{
    try{
        return getBestChordProgression(ruleFile,ruleNames,ml,m,bis);
    }
    catch(std::exception& e)
    {
        throw MyException(e);
    }

}

void MainWindow::computationFinished()
{
    ui->GenerateChords->setEnabled(true);
    ui->actionGenerate_Chords->setEnabled(true);
    try{

        currentChordProgression = future.result();
        initialize_Player();
     }
    catch(MyException& me)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_Computation_finished released");
        messageBox.setText(me.msg());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_Computation_finished unknown");
        messageBox.exec();
    }
}



void MainWindow::on_GenerateChords_clicked()
{
    try
    {

        std::string ruleFile = "default.rules";
        std::vector<std::string> ruleNames = { "Default1", "Default2", "Default3", "Default4", "Default5"};
        std::vector<std::string> simpleChords = {};
        MachineLearningOutput ml/* = getExhaustiveChordProgression(simpleChords, currentMelody.size())*/;


        connect(&watcher,SIGNAL(finished()),this,SLOT(computationFinished()));
        //connect(&watcher,SIGNAL(canceled()),ui->GenerateChords,SLOT(setChecked(true;)));

        future = QtConcurrent::run(computeChordsWrapper,ruleFile, ruleNames, ml, currentMelody, 1);

        watcher.setFuture(future);
        ui->GenerateChords->setEnabled(false);
        ui->actionGenerate_Chords->setEnabled(false);

        //            currentChordProgression = getBestChordProgression(ruleFile, ruleNames, ml, currentMelody, 1);

        //           initialize_Player();

        //            ui->GenerateChords->setChecked(false);

    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_GenerateChords_released");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_GenerateChords_released unknown");
        messageBox.exec();
    }
}

void MainWindow::on_Tempo_valueChanged(int bpm)
{
    try
    {
        currentPlayer.setBPM(static_cast<unsigned>(bpm));
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_Tempo_valueChanged");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_Tempo_valueChanged unknown");
        messageBox.exec();
    }
}

void MainWindow::on_VolumeMelody_valueChanged(int value)
{
    try
    {
        int marge = ui->VolumeMelody->maximum() - ui->VolumeMelody->minimum() + 1;
        currentPlayer.setMelodyVolume(static_cast<float>(value) / static_cast<float>(marge));

        if (!(ui->MuteMelody->isChecked()))
        {
            if (value == 0)
                ui->MuteMelody->setIcon(QIcon(":icons/vol0.png"));
            else if (value < marge / 3)
                ui->MuteMelody->setIcon(QIcon(":icons/vol1.png"));
            else if (value < 2 * (marge / 3))
               ui->MuteMelody->setIcon(QIcon(":icons/vol2.png"));
            else
                ui->MuteMelody->setIcon(QIcon(":icons/vol3.png"));
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_VolumeMelody_valueChanged");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_VolumeMelody_valueChanged unknown");
        messageBox.exec();
    }
}

void MainWindow::on_VolumeChords_valueChanged(int value)
{
    try
    {
        int marge = ui->VolumeChords->maximum() - ui->VolumeChords->minimum() + 1;
        currentPlayer.setChordVolume(static_cast<float>(value) / static_cast<float>(marge));

        if (!(ui->MuteChords->isChecked()))
        {
            if (value == 0)
                ui->MuteChords->setIcon(QIcon(":icons/vol0.png"));
            else if (value < marge / 3)
                ui->MuteChords->setIcon(QIcon(":icons/vol1.png"));
            else if (value < 2 * (marge / 3))
                ui->MuteChords->setIcon(QIcon(":icons/vol2.png"));
            else
                ui->MuteChords->setIcon(QIcon(":icons/vol3.png"));
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_VolumeChords_valueChanged");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_VolumeChords_valueChanged unknown");
        messageBox.exec();
    }
}

void MainWindow::on_MuteMelody_toggled(bool checked)
{
    try
    {
        if (checked)
        {
            currentPlayer.muteMelody();
            ui->MuteMelody->setIcon(QIcon(":icons/volmute.png"));
        }
        else
        {
            currentPlayer.unmuteMelody();
            on_VolumeMelody_valueChanged(ui->VolumeMelody->value());
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_MuteMelody_toggled");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_MuteMelody_toggled unknown");
        messageBox.exec();
    }
}

void MainWindow::on_MuteChords_toggled(bool checked)
{
    try
    {
        if (checked)
        {
            currentPlayer.muteChord();
            ui->MuteChords->setIcon(QIcon(":icons/volmute.png"));
        }
        else
        {
            currentPlayer.unmuteChord();
            on_VolumeChords_valueChanged(ui->VolumeChords->value());
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_MuteChords_toggled");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_MuteChords_toggled unknown");
        messageBox.exec();
    }
}

void MainWindow::on_Play_toggled(bool checked)
{
    try
    {
        if (checked)
            currentPlayer.play();
        else
            currentPlayer.pause();
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_Play_toggle");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_Play_toggle unknown");
        messageBox.exec();
    }
}

void MainWindow::on_Stop_released()
{
    try
    {
        if (ui->Play->isChecked())
        {
            ui->Play->setChecked(false);
        }
        //currentPlayer.setPosition(Time(0));
        currentPlayer.stop();
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_Stop_Released");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_Stop_Released unknown");
        messageBox.exec();
    }
}

void MainWindow::on_Repeat_toggled(bool checked)
{
    try
    {
        currentPlayer.setRepeat(checked);
    }
    catch(const std::exception& e)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Exception in on_Repeat_toggled");
        messageBox.setText(e.what());
        messageBox.exec();
    }
    catch(...)
    {
        QMessageBox messageBox;
        messageBox.setText("Exception in on_Repeat_toggled unknown");
        messageBox.exec();
    }
}

void MainWindow::initialize_Player()
{
    currentPlayer.setSong(currentMelody, currentChordProgression);

    unsigned bpm = currentPlayer.getBPM();
    if (bpm < 10) {
        ui->Tempo->setValue(10);
        currentPlayer.setBPM(10);
    }else if (bpm >250){
        ui->Tempo->setValue(250);
        currentPlayer.setBPM(250);
    }else{
        ui->Tempo->setValue(static_cast<int>(bpm));
    };

    on_VolumeMelody_valueChanged(ui->VolumeMelody->value());
    on_VolumeChords_valueChanged(ui->VolumeChords->value());

    if (ui->MuteMelody->isChecked())
        on_MuteMelody_toggled(true);
    if (ui->MuteChords->isChecked())
        on_MuteChords_toggled(true);

    if (ui->Play->isChecked())
        ui->Play->setChecked(false);
}

void MainWindow::on_actionAbout_the_project_triggered()
{
    QMessageBox messageBox;
    messageBox.setText("Project Lazychords\n Under GNU GPL v2 License\n Website :  http://perso.ens-lyon.fr/valentin.le-fevre/lazychords/ \n Credits to :\n FMOD : www.fmod.org \n Icons : Sylvain Amatoury & useiconic.com (Noun Project)");
    messageBox.exec();
}
