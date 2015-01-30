#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include<QMessageBox>
#include <QObject>
#include <QTimer>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>
#include <climits>
#include <QFuture>
#include "myexception.h"
#include "Music.hpp"
#include "MusicRules.hpp"
#include "Track.hpp"
#include "Utilities.hpp"
namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    MainWindow(const MainWindow&)  = delete;
    MainWindow operator=(const MainWindow&) = delete;
                                                   
              

private slots:
    void togglePlay();

    void on_ImportABC_released();

    void on_GenerateChords_clicked();

    void on_Tempo_valueChanged(int arg1);

    void on_VolumeMelody_valueChanged(int value);

    void on_VolumeChords_valueChanged(int value);

    void on_MuteMelody_toggled(bool checked);

    void on_MuteChords_toggled(bool checked);

    void on_Play_toggled(bool checked);

    void on_Stop_released();

    void computationFinished();

    void on_Repeat_toggled(bool checked);

    void initialize_Player();

    void on_actionAbout_the_project_triggered();

    void updatePlayer();

private:
    Ui::MainWindow *ui;

    Melody currentMelody;
    ChordProgression currentChordProgression;
    SongPlayer currentPlayer;
    QTimer timer;
    QFutureWatcher<ChordProgression> watcher;
    QFuture<ChordProgression> future;
};

#endif // MAINWINDOW_H
