import cv2
import numpy

def write_MNIST_files():

	file_object = open('../data/data.csv', 'r')

	file_object.readline()

	counters = {_ : 0 for _ in range(10)}

	folders = {
		0 : 'zero', 1 : 'one', 2 : 'two', 3 : 'three',
		4 : 'four', 5 : 'five', 6 : 'six', 7 : 'seven',
		8 : 'eight', 9 : 'nine'
	}

	for line in file_object:

		parsed = map(lambda x : int(x.strip()), line.split(','))
		label = int(parsed[0])

		image_array = numpy.array(parsed[1:])
		image_array = image_array.reshape(28, 28)

		imagefilename = "../img/data/" + folders[label] + "/file" + "_" + str(counters[label]) + ".png"
		cv2.imwrite(imagefilename, image_array)
		counters[label] = counters[label] + 1