class Solution(object):
    def permute(self, nums):
        """
        :type nums: List[int]
        :rtype: List[List[int]]
        """
        if not nums:
            return [nums]
        else:
            result = []
            for i in range(len(nums)):
                r = self.permute(nums[:i] + nums[i + 1:])
                for x in r:
                    result.append(nums[i:i+1] + x)

            return result

print(Solution().permute([1, 2, 3]))
