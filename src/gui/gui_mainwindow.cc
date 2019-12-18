//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
\file    gui_mainwindow.cc
\brief   GUI for F1/10 car.
\author  Joydeep Biswas, (C) 2019
*/
//========================================================================

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "gui_mainwindow.h"

#include <string>
#include <vector>

#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QWidget>

#include "ros/ros.h"
#include "std_msgs/String.h"

using std::string;
using std::vector;

vector<string> GetIPAddresses() {
  static const bool kGetIPV6 = false;
  vector<string> ips;
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) {
        continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET) {
      // is a valid IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      ips.push_back(
          string(ifa->ifa_name) + " : " + string(addressBuffer));
    } else if (kGetIPV6 && ifa->ifa_addr->sa_family == AF_INET6) {
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      char addressBuffer[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
      ips.push_back(
          string(ifa->ifa_name) + " : " + string(addressBuffer));
    }
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  return ips;
}


namespace f1tenth_gui {

MainWindow::MainWindow(ros::NodeHandle* node_handle, QWidget* parent) :
    time_label_(nullptr),
    vector_display_(nullptr),
    robot_label_(nullptr),
    main_layout_(nullptr) {
  this->setWindowTitle("F1/10 GUI");
  robot_label_ = new QLabel("F1/10");
  QFont font("Arial");
  font.setPointSize(60);
  robot_label_->setFont(font);
  robot_label_->setAlignment(Qt::AlignCenter);

  QPushButton* close_button = new QPushButton("Close");
  close_button->setFocusPolicy(Qt::NoFocus);
  QHBoxLayout* top_bar = new QHBoxLayout();
  time_label_ = new QLabel("00:00 AM");
  time_label_->setWordWrap(true);
  QLabel* status_label = new QLabel("Mode: Autonomous");
  top_bar->addWidget(status_label);
  top_bar->addStretch();
  top_bar->addWidget(time_label_);
  top_bar->addStretch();
  top_bar->addWidget(close_button);

  vector_display_ = new QWidget();

  main_layout_ = new QVBoxLayout(this);
  setLayout(main_layout_);
  main_layout_->addLayout(top_bar, Qt::AlignTop);
  main_layout_->addWidget(vector_display_);
  main_layout_->addWidget(robot_label_, Qt::AlignCenter);
  // robot_label_->hide();
  vector_display_->hide();

  connect(close_button, SIGNAL(clicked()), this, SLOT(closeWindow()));
  connect(this,
          SIGNAL(UpdateSignal()),
          this,
          SLOT(UpdateDisplay()));

  QTimer* time_update_timer = new QTimer(this);
  connect(time_update_timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));
  time_update_timer->start(1000);
  UpdateTime();
}

void MainWindow::closeWindow() {
  close();
}

void MainWindow::UpdateTime() {
  const vector<string> ips = GetIPAddresses();
  string s;
  for (const string& ip : ips) {
    s = s + ip + "\n";
  }
  time_label_->setText(QString::fromUtf8(s.c_str()));
  // time_label_->setText(QTime::currentTime().toString("hh:mm AP"));
}



}  // namespace f1tenth_gui
