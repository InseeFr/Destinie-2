FROM r-base:3.6.2

ENV LC_ALL C.UTF-8

# install dependencies
RUN apt update -yq && \
  apt install -y libssl-dev libcurl4-openssl-dev libxml2-dev

# install Java
# Install OpenJDK-8
RUN apt-get update && \
    apt-get install -y r-cran-rjava

# Fix certificate issues
RUN apt-get update && \
    apt-get install ca-certificates-java && \
    apt-get clean && \
    update-ca-certificates -f

# Setup JAVA_HOME -- useful for docker commandline
ENV JAVA_HOME /usr/lib/jvm/default-java/

COPY . /destinie2
WORKDIR /destinie2

VOLUME /destinie2/data

RUN Rscript -e "install.packages(c('ggplot2', 'tidyr', 'dplyr', 'openxlsx', 'xlsx','jsonlite'))"

RUN R CMD INSTALL . --preclean --debug
