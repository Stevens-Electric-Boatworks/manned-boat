import random

class SmoothRandom:
    def __init__(self, start, step, low, high):
        self.value = start
        self.step = step
        self.low = low
        self.high = high

    def next(self):
        change = random.uniform(-self.step, self.step)
        self.value = min(max(self.value + change, self.low), self.high)
        return self.value