#include "mainwindow.h"
#include "ui_mainwindow.h"

//sudo apt-get install libqt5svg5*

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Update all the text with what is currently in the targeted files
    QFile reward_func_file(reward_func_path);
    if(!reward_func_file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open reward function file: " + reward_func_file.errorString());
    } else {
        QTextStream in(&reward_func_file);
        current_reward_func = in.readAll();
        reward_func_file.close();
    }

    QFile action_space_file(action_space_path);
    if(!action_space_file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open action space file: " + action_space_file.errorString());
    } else {
        QTextStream in(&action_space_file);
        current_action_space = in.readAll();
        action_space_file.close();
    }


    QFile hyperparameters_file(hyperparameters_path);
    if(!hyperparameters_file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open hyperparameters file: " + hyperparameters_file.errorString());
    } else {
        QTextStream in(&hyperparameters_file);
        QString rl_file(in.readAll());
        QString text("Hyperparameters:\n");
        for(int i=0;i<hyperparams.length();i++){
            text += hyperparams[i] + " : ";
            int x=rl_file.indexOf(hyperparams[i])+hyperparams[i].length()+2; //Starts with + 2 because of "": #######," format
            while(rl_file.at(x) != '\n' && rl_file.at(x) != ','){
                text += rl_file.at(x);
                x++;
            }
            text += "\n";
        }
        current_hyperparameters = text;
        hyperparameters_file.close();
    }


    QFile track_file(track_path);
    if(!track_file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open track file: " + track_file.errorString());
    } else {
        QTextStream in(&track_file);
        current_track = in.readLine(); //First line contains track
        track_file.close();
    }

    //Set all the text on the GUI to the updated strings
    ui->reward_function->setText(current_reward_func);
    ui->action_space->setText(current_action_space);
    ui->hyper_parameters->setText(current_hyperparameters);
    ui->track_name->setText(current_track);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_button_clicked()
{
    //Start the simulation and training instance

}

void MainWindow::on_save_button_clicked()
{
    //Save updates writen to the QText editor to the minio bucket and coach_rl python file
    //Update all the text with what is currently in the TextEdit

    QFile reward_func_file(reward_func_path);
    if(!reward_func_file.open(QIODevice::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open reward function file: " + reward_func_file.errorString());
    } else {
        QTextStream out(&reward_func_file);
        out << ui->reward_function->toPlainText();
        reward_func_file.close();
    }

    QFile action_space_file(action_space_path);
    if(!action_space_file.open(QIODevice::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open action space file: " + action_space_file.errorString());
    } else {
        QTextStream out(&action_space_file);
        out << ui->action_space->toPlainText();
        action_space_file.close();
    }


    QFile hyperparameters_file(hyperparameters_path);
    if(!hyperparameters_file.open(QIODevice::ReadWrite | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open hyperparameters file: " + hyperparameters_file.errorString());
    } else {
        //Read existing text in the file
        QTextStream in(&hyperparameters_file);
        QString rl_file = in.readAll(); //First line contains track

        //Read the new track from the GUI
        QString text(ui->hyper_parameters->toPlainText());

        //Edit the file text
        for(int i=0;i<hyperparams.length();i++){
            QString re(hyperparams[i]);
            int param_index = rl_file.indexOf(hyperparams[i])+hyperparams[i].length();
            while(rl_file[param_index] != '\n'){
                re+=rl_file[param_index];
                param_index++;
            }
            QString new_value(hyperparams[i]+"\":");
            param_index = text.indexOf(hyperparams[i])+hyperparams[i].length()+3;
            while(text[param_index] != '\n'){
                new_value+=text[param_index];
                param_index++;
            }
            if(hyperparams[i] != "term_cond_avg_score"){
                new_value += ",";
            }
            rl_file.replace(re,new_value);
        }

        //Write edited text back into file
        QTextStream out(&hyperparameters_file);
        hyperparameters_file.resize(0); //clear the existing file
        out << rl_file; //First line contains new track name
        hyperparameters_file.close();
    }


    QFile track_file(track_path);
    if(!track_file.open(QIODevice::ReadWrite | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open track file: " + track_file.errorString());
    } else {
        //Read existing text in the file
        QTextStream in(&track_file);
        QString env_file = in.readAll(); //First line contains track

        //Read the new track from the GUI
        QString replacementText(ui->track_name->text());

        //Edit the file text
        QString first_line = "";
        int i = 0;
        while(env_file[i] !='\n'){
            first_line += env_file[i];
            i++;
        }
        QRegularExpression re(first_line);
        env_file.replace(re, replacementText);

        //Write edited text back into file
        QTextStream out(&track_file);
        track_file.resize(0); //clear the existing file
        out << env_file; //First line contains new track name
        track_file.close();
    }

}

void MainWindow::on_restart_button_clicked()
{
    //Restart the simulation and training instance using model that has been training (ie using pretrained model)
    //This allows you to tweak the parameters incrementally
    current_action_space.clear();
    current_action_space.clear();
}

void MainWindow::on_stop_button_clicked()
{
    //Stop the training instance

}