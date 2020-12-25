close all
clear, clc

% figure();
% ideal = load('file.txt');
% xi = ideal(:,1);
% yi = ideal(:,2);
% plot(xi,yi,'r');
% grid on; hold on;

figure();
ideal = load('file2.txt');
xi = ideal(:,1);
yi = ideal(:,2);
plot(xi,yi,'r');
ylim([-1,1]);
grid on; hold on;