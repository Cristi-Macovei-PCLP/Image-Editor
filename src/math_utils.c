// Macovei Nicolae-Cristian
// Anul I, grupa 312CAb

int clamp(int value, int left, int right)
{
	if (value < left)
		return left;

	if (value > right)
		return right;

	return value;
}
