packageName=$1
sampleName=$2

echo Copying Packages
cp -r com.unity.renderstreaming EmptyProjectForTest~/Packages

echo Copying ${sampleName} Sample from ${packageName}
mkdir -p EmptyProjectForTest~/Assets/Samples
cp -r ${packageName}/Samples~/${sampleName} EmptyProjectForTest~/Assets/Samples

echo Copying ${sampleName} Finished
