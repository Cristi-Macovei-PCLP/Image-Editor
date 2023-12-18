int clamp(int value, int left, int right) {
  if (value < left)
    return left;

  if (value > right)
    return right;

  return value;
}