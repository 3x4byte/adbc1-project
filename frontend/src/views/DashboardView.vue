<template>
  <div>
    <h1>Willkommen im Dashboard!</h1>
    <button @click="logout">Abmelden</button>

    <VueDatePicker v-model="dateRange" @update:model-value="handleNewDate" range time-picker-inline />
    <br>
    <LineChart :chart-data="chartData"/>
  </div>
</template>

<script>
import LineChart from '@/components/LineChart.vue';

import VueDatePicker from '@vuepic/vue-datepicker';
import '@vuepic/vue-datepicker/dist/main.css'

export default {
  name: 'ChartView',
  components: {
    LineChart,
    VueDatePicker
  },
  data() {
    return {
      dateRange: null,
      chartData: {
        labels: [],
        datasets: [
          {
            label: "temperature",
            backgroundColor: '#ff0000',
            borderColor: '#ff0000',
            data: []
          },
          {
            label: "humidity",
            backgroundColor: '#0000ff',
            borderColor: '#0000ff',
            data: []
          },
          {
            label: "air_pressure",
            backgroundColor: '#00ff00',
            borderColor: '#00ff00',
            data: []
          },
        ],
      },
    }
  },
  methods: {
    async getData(start, end){
      let formattedStart = start.getFullYear() + '-' 
                   + String(start.getMonth() + 1).padStart(2, '0') + '-' 
                   + String(start.getDate()).padStart(2, '0') + ' ' 
                   + String(start.getHours()).padStart(2, '0') + ':' 
                   + String(start.getMinutes()).padStart(2, '0') + ':' 
                   + String(start.getSeconds()).padStart(2, '0');

      let formattedEnd = end.getFullYear() + '-' 
                   + String(end.getMonth() + 1).padStart(2, '0') + '-' 
                   + String(end.getDate()).padStart(2, '0') + ' ' 
                   + String(end.getHours()).padStart(2, '0') + ':' 
                   + String(end.getMinutes()).padStart(2, '0') + ':' 
                   + String(end.getSeconds()).padStart(2, '0');

      const list = await this.$pb.collection("data").getFullList({
        filter: `created > '${formattedStart}' && created < '${formattedEnd}'`
      });

      for (const item of list) {
        this.appendData(item.created, item.temperature, item.humidity, item.air_pressure)
      }

    },

    appendData(label, temperature, humidity, air_pressure){
      this.chartData = {
        labels: [...this.chartData.labels, label],
        datasets: [
          {
            label: "temperature",
            backgroundColor: '#ff0000',
            borderColor: '#ff0000',
            data: [...this.chartData.datasets[0].data, temperature],
          },
          {
            label: "humidity",
            backgroundColor: '#0000ff',
            borderColor: '#0000ff',
            data: [...this.chartData.datasets[1].data, humidity],
          },
          {
            label: "air_pressure",
            backgroundColor: '#00ff00',
            borderColor: '#00ff00',
            data: [...this.chartData.datasets[2].data, air_pressure],
          },
        ],
      };

    },
    
    clearData(){
      this.chartData = {
        labels: [],
        datasets: [
          {
            label: "temperature",
            backgroundColor: '#ff0000',
            borderColor: '#ff0000',
            data: []
          },
          {
            label: "humidity",
            backgroundColor: '#0000ff',
            borderColor: '#0000ff',
            data: []
          },
          {
            label: "air_pressure",
            backgroundColor: '#00ff00',
            borderColor: '#00ff00',
            data: []
          },
        ],
      };

    },

    handleNewDate(modelDate){
      this.clearData()
      let start = new Date(modelDate[0])
      let end = new Date(modelDate[1])
      this.getData(start, end)
    },

    async logout() {
      await this.$pb.authStore.clear()
      this.$router.push('/login'); // Weiterleiten
    }

  },
  mounted() {

    let now = new Date()
    let yesterday = new Date()
    yesterday.setDate(now.getDate() - 1)

    this.dateRange = [ yesterday.toISOString(), now.toISOString() ]
    this.getData(yesterday, now);
  }
}
</script>
