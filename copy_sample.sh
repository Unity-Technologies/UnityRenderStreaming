packageName=$1
sampleName=$2

echo Copying Packages
cp -r Packages/com.unity.renderstreaming EmptyProjectForTest~/Packages

echo Copying ${sampleName} Sample from ${packageName}
mkdir -p EmptyProjectForTest~/Assets/Samples
cp -r Packages/${packageName}/Samples~/${sampleName} EmptyProjectForTest~/Assets/Samples

echo Copying ${sampleName} Finished
