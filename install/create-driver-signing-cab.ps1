# Create a driver signing .cab suitable for submission to the hardware
# dev center.
# https://msdn.microsoft.com/en-us/library/windows/hardware/dn962252.aspx

# We need a unique folder name. For now just date-stamp it.
$cab_version = Get-Date -Format yyyy_MM_dd_HH_mm_ss
$cab_dir = "winpcap_driver_signing_$cab_version"
$cab_x86_dir = "$cab_dir\x86"
$cab_x64_dir = "$cab_dir\x64"
$driver_x86_dir = "..\packetNtx\driver\bin\i386"
$driver_x64_dir = "..\packetNtx\driver\bin\amd64"

write-host "Creating $cab_dir"

new-item -ItemType Directory -Path $cab_dir
new-item -ItemType Directory -Path $cab_x86_dir
new-item -ItemType Directory -Path $cab_x64_dir

copy-item "npfx86.inf" -Destination "$cab_x86_dir\npf.inf"
copy-item "$driver_x86_dir\npf.sys" -Destination $cab_x86_dir
copy-item "$driver_x86_dir\npf.pdb" -Destination $cab_x86_dir

copy-item "npfx64.inf" -Destination "$cab_x64_dir\npf.inf"
copy-item "$driver_x64_dir\npf.sys" -Destination $cab_x64_dir
copy-item "$driver_x64_dir\npf.pdb" -Destination $cab_x64_dir

# http://stackoverflow.com/questions/19411440/makecab-create-a-cab-file-from-all-the-files-in-a-folder

function compress-directory([string]$dir, [string]$output)
{
    $ddf = ".OPTION EXPLICIT
.Set CabinetNameTemplate=$output
.Set DiskDirectory1=.
.Set CompressionType=MSZIP
.Set Cabinet=on
.Set Compress=on
.Set CabinetFileCountThreshold=0
.Set FolderFileCountThreshold=0
.Set FolderSizeThreshold=0
.Set MaxCabinetSize=0
.Set MaxDiskFileCount=0
.Set MaxDiskSize=0
"
    $dirfullname = (get-item $dir).fullname
    $ddfpath = ($env:TEMP+"\temp.ddf")
    $ddf += (ls -recurse $dir | ? {!$_.psiscontainer}|select -expand fullname|%{'"'+$_+'" "'+$_.SubString($dirfullname.length+1)+'"'}) -join "`r`n"
    $ddf
    $ddf | Out-File -encoding UTF8 $ddfpath
    makecab /F $ddfpath
    rm $ddfpath
    rm setup.inf
    rm setup.rpt
}

compress-directory ".\$cab_dir" ".\$cab_dir.cab"
