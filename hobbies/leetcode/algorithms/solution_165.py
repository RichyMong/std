class Solution(object):
    def compareVersion(self, version1, version2):
        """
        :type version1: str
        :type version2: str
        :rtype: int
        """
        v1 = [int(s) for s in version1.split('.')]
        v2 = [int(s) for s in version2.split('.')]

        if len(v1) < len(v2):
            v1.extend([0] * (len(v2) - len(v1)))
        elif len(v2) < len(v1):
            v2.extend([0] * (len(v1) - len(v2)))

        if v1 > v2: return 1

        elif v1 < v2: return -1

        else: return 0

s = Solution()
print(s.compareVersion('1.0', '1'))
print(s.compareVersion('1.1', '1.2'))
print(s.compareVersion('1.0.0', '1'))
print(s.compareVersion('1.01', '1.1'))
