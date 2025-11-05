using Simulation;
using Simulation.Multithreaded;
using Simulation.Stepwise;

var simulation = new MultithreadedSimulation();
simulation.Run(300_000);