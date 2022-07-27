from dataclasses import dataclass

import matplotlib.pyplot as plt
import numpy as np
import math
import datetime
import random

@dataclass
class Coordinate:
    x: float
    y: float
    z: float


def stringToCoordinate(elem):
    elem = elem[:-2]
    elem = elem[1:]
    coords = elem.split(',')
    return Coordinate(float(coords[0]), float(coords[1]), float(coords[2]))


def parseLogFile(file):
    if not file:
        return {}
    log_data = {}
    file1 = open(file, 'r')
    lines = file1.readlines()
    i = 0
    times = []
    for line in lines:
        line_stripped = line.replace(' ', '')
        line_stripped = line_stripped.replace('\n', '')
        line_stripped = line_stripped.split('|')

        time = datetime.datetime.strptime(line_stripped[0], '%d-%m-%Y_%H-%M-%S')
        if i == 2:
            times.append(time)
            i = 0
        else:
            i += 1

        data = line_stripped[1].split(':', 1)
        if data[0] not in log_data:
            log_data[data[0]] = []
        log_data[data[0]].append(data[1])
    log_data['time'] = times
    return log_data


def convertToCSV(file, data):
    f = open(file, "w")
    f.write("Time,VirtualPosition,RealPosition,Recommendation\n")
    for i in range(0, len(data['Receivedvirtpos'])):
        f.write(f'"{str(data["time"][i])}","{str(data["Receivedvirtpos"][i])}",'
                f'"{str(data["Receivedrealpos"][i])}",{str(data["RecommendRotation"][i])}\n')
    f.close()


def getRotStats(rotations, file):
    rots = []
    resets = []
    for x in rotations:
        if 'RESET' in x:
            y = x.split(':')[1]
            resets.append(float(y))
        else:
            num = float(x)
            if num in rots:
                continue
            rots.append(num)
    print("Average rot: ", np.average(rots))
    print("Minimum rot: ", np.min(rots))
    print("Maximum rot: ", np.max(rots))
    print("# Resets: ", len(resets))
    file.write("Average rot: " + str(np.average(rots)) + '\n')
    file.write("Minimum rot: " + str(np.min(rots)) + '\n')
    file.write("Maximum rot: " + str(np.max(rots)) + '\n')
    file.write("# Resets: " + str(len(resets)) + '\n')


def distanceBetweenResets(rotations, positions, file):
    distances = []
    distance = 0
    for x in range(len(positions)-1):
        coordinate_1 = stringToCoordinate(positions[x])
        coordinate_2 = stringToCoordinate(positions[x+1])
        distance += math.sqrt(pow((coordinate_2.x - coordinate_1.x), 2) + pow((coordinate_2.y - coordinate_1.y), 2))
        if 'RESET' in rotations[x+1] and 'RESET' not in rotations[x]:
            distances.append(distance)
            distance = 0
    print("Avg. distance between resets: ", np.average(distances))
    print("Min. distance between resets: ", np.min(distances))
    print("Max. distance between resets: ", np.max(distances))
    file.write("Avg. distance between resets: " + str(np.average(distances)) + '\n')
    file.write("Min. distance between resets: " + str(np.min(distances)) + '\n')
    file.write("Max. distance between resets: " + str(np.max(distances)) + '\n')


def otherStats(times, file):
    length = times[-1] - times[0]
    file.write("Simulation duration: " + str(length) + '\n')


def printMovement(size, pos_1, pos_2, max_points):
    points = 0
    x_coord_1 = []
    y_coord_1 = []
    x_coord_2 = []
    y_coord_2 = []
    smallest = len(pos_1) if len(pos_1) < len(pos_2) else len(pos_2) if len(pos_2) != 0 else len(pos_1)
    for x in range(smallest):
        if points >= max_points:
            break
        coordinate_1 = stringToCoordinate(pos_1[x])
        x_coord_1.append(coordinate_1.x)
        y_coord_1.append(coordinate_1.z)
        if pos_2:
            coordinate_2 = stringToCoordinate(pos_2[x])
            x_coord_2.append(coordinate_2.x)
            y_coord_2.append(coordinate_2.z)
        points += 1

    b = size/2
    plt.plot([b, b, -b, -b, b], [b, -b, -b, b, b], 'r-', label='Border')
    plt.plot([0], [0], 'yo')
    plt.plot(x_coord_1, y_coord_1, 'b-.', label='Path User 1')
    if x_coord_2:
        plt.plot(x_coord_2, y_coord_2, 'g-.', label='Path User 2')
    plt.axis('equal')
    plt.legend(loc='upper left')
    plt.title('User paths during 5min experiment')
    plt.show()



def scatter(data):
    x_coord = []
    y_coord = []
    for x, y in data:
        x_coord.append(x)
        y_coord.append(y)

    plt.scatter(x=x_coord, y=y_coord)
    plt.scatter(x=[-0.2], y=[-0.3], c='#ff7f0e')
    plt.show()


def stats(user, data, file):
    print('Stats ' + user + ': ')
    file.write('Stats ' + user + ':\n')
    getRotStats(data['RecommendRotation'], file)
    distanceBetweenResets(data['RecommendRotation'], data['Receivedvirtpos'], file)
    otherStats(data['time'], file)
    file.write('\n')


if __name__ == '__main__':
    room_size = 10
    data_1 = parseLogFile('data/S10/192.168.43.151_13-07-2022_12-30-08.txt')
    convertToCSV('data/S10/192.168.43.151_13-07-2022_12-30-08.csv', data_1)
    data_2 = None
    try:
        data_2 = parseLogFile('data/S10/192.168.43.33_13-07-2022_12-29-43.txt')
        convertToCSV('data/S10/192.168.43.33_13-07-2022_12-29-43.csv', data_2)
    except:
        pass
    output = open('data/S10/stats.txt', 'w')
    if not data_2:
        printMovement(room_size, data_1['Receivedvirtpos'], {}, 1000000)
        stats('User 1', data_1, output)
    if data_2:
        printMovement(room_size, data_1['Receivedvirtpos'], data_2['Receivedvirtpos'], 1000000)
        stats('User 1', data_1, output)
        stats('User 2', data_2, output)

