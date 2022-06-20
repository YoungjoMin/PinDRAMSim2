log1 = "dramsim_with_RAIDR.log"
log2 = "dramsim_without_RAIDR.log"
out1 = "dramsim_with_RAIDR.out"
out2 = "dramsim_without_RAIDR.out"

def analyze(inp, out):
  cnt=0
  totalE = 0.0
  totalBackgroundE = 0.0
  totalActE = 0.0
  totalBurstE = 0.0
  totalRefreshE = 0.0
  totalRefreshCnt = 0
  lines = inp.readlines()
  for line in lines:
    if line.find("============== Printing Statistics [id:0]==============") != -1:
      totalRefreshCnt = 0
      continue
    if line.find("Average Power") != -1:
      totalE += float(line.split()[-1])
      cnt += 1
      continue
    if line.find("-Background") != -1:      
      totalBackgroundE += float(line.split()[-1])
      continue
    if line.find("-Act/Pre") != -1:
      totalActE += float(line.split()[-1])
      continue
    if line.find("-Burst") != -1:
      totalBurstE += float(line.split()[-1])
      continue
    if line.find("-Refresh") != -1:
      totalRefreshE += float(line.split()[-1])
      continue
    if line.find("total refresh cnt") != -1:
      totalRefreshCnt += int(line.split()[-1])
  out.write("totalE : %f\n" % (totalE/cnt))
  out.write("totalBackgroundE : %f\n" % (totalBackgroundE/cnt))
  out.write("totalActE : %f\n" % (totalActE/cnt))
  out.write("totalBurstE : %f\n" % (totalBurstE/cnt))
  out.write("totalRefreshE : %f\n" % (totalRefreshE/cnt))
  out.write("totalRefreshCnt : %d\n" % (totalRefreshCnt))
  return

with open(log1,"rt") as inp:
  with open(out1, "wt") as out:
    analyze(inp, out)

with open(log2,"rt") as inp:
  with open(out2,"wt") as out:
    analyze(inp, out)
